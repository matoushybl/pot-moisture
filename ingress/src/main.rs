use axum::{http::StatusCode, response::IntoResponse, routing::post, Extension, Json, Router};
use clap::Parser;
use std::{
    net::{Ipv4Addr, SocketAddr},
    sync::{Arc, Mutex},
};
use telegraf::{Client, Metric};

#[derive(Metric)]
struct Wetness {
    #[telegraf(tag)]
    plant_name: String,
    value: i32,
}

use serde::Deserialize;

#[derive(Debug, Deserialize)]
struct RawData {
    name: String,
    value: i32,
}

#[derive(Parser, Debug)]
struct Cli {
    #[clap(long, default_value_t = String::from("localhost:8094"))]
    telegraf_url: String,
}

const PORT: u16 = 8085;

#[tokio::main]
async fn main() -> eyre::Result<()> {
    tracing_subscriber::fmt::init();

    let cli = Cli::try_parse()?;

    let client = Arc::new(Mutex::new(
        Client::new(&format!("tcp://{}", &cli.telegraf_url)).unwrap(),
    ));

    let app = Router::new()
        .route("/ingress", post(handle_device_data))
        .layer(Extension(client));

    println!(
        "Running pot moisture ingress on port {}, relaying data to telegraf on {}.",
        PORT, cli.telegraf_url
    );
    axum::Server::bind(&SocketAddr::new(Ipv4Addr::UNSPECIFIED.into(), PORT))
        .serve(app.into_make_service())
        .await?;
    Ok(())
}

async fn handle_device_data(
    Json(body): Json<RawData>,
    Extension(telegraf): Extension<Arc<Mutex<Client>>>,
) -> impl IntoResponse {
    println!("Received payload: {:?}", body);
    telegraf
        .lock()
        .unwrap()
        .write(&Wetness {
            plant_name: body.name,
            value: body.value,
        })
        .unwrap();
    (StatusCode::OK, "hello")
}
