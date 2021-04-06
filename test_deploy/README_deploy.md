# Docker deploy

## Purpose
Being able to quickly verify that the application is building on various distros with various Qt version.

## Prerequisites
- docker
- docker-compose

## How?
Simply run `deploy.sh` as root (e.g. with sudo; needed for docker commands by default).
Then just select which of the available distros (specific one or all) shall be built.
Summary of build will be shown at end.

## What's actually happening behind the scenes?
Basically for a given selection, a docker container is being started, which then will perform a build 
based on the local repository content.
