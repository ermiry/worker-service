## General
- Added custom errors definitions & methods
- Also added dedicated missing values HTTP response
- Added base mongo configuration in main
- Added base service state definitions & methods
- Added redis configuration in main methods
- Added redis related sources in Dockerfiles
- Updated compilation dependencies in makefile

## Models
- Added base transaction model implementation
- Added time & result fields in trans model

## Controller
- Added dedicated transactions controller

## Routes
- Added base transactions routes handlers
- Added base routes to fetch service data
- Added base routes to interact with service state
- Added dedicated worker related routes

## Worker
- Added base service worker custom methods
- Added base worker thread method logic
- Added dedicated method to register worker

## Data
- Added dedicated service data methods
- Added base methods to fetch values from backup
- Added complete process time data values
