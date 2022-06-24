FROM rust:latest as builder

RUN USER=root cargo new --bin ingress
WORKDIR ./ingress
COPY ./Cargo.toml ./Cargo.toml
COPY ./Cargo.lock ./Cargo.lock
RUN cargo build --release

RUN rm src/*.rs
ADD ./src ./src

# why tho?
RUN rm ./target/release/deps/ingress* 
RUN cargo build --release


FROM debian:bullseye-slim

ARG APP=/usr/src/app

RUN apt-get update \
    && apt-get install -y ca-certificates tzdata \
    && rm -rf /var/lib/apt/lists/*

EXPOSE 8000

ENV APP_USER=appuser \
    TELEGRAF_URL=localhost:8094

RUN groupadd $APP_USER \
    && useradd -g $APP_USER $APP_USER \
    && mkdir -p ${APP}

COPY --from=builder /ingress/target/release/ingress ${APP}/ingress

RUN chown -R $APP_USER:$APP_USER ${APP}

USER $APP_USER
WORKDIR ${APP}

CMD ./ingress --telegraf-url $TELEGRAF_URL
