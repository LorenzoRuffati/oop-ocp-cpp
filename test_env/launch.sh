#!/bin/bash
docker-compose up -d --build
docker-compose exec dev-env bash
# docker-compose down -t 2