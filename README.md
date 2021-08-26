# Worker Test Service

### Development
```
sudo docker run \
  -it \
  --name service --rm \
  -p 5010:5010 --net worker \
  -v /home/ermiry/Documents/ermiry/Projects/worker-service:/home/worker \
  -e RUNTIME=development \
  -e PORT=5010 \
  -e CERVER_RECEIVE_BUFFER_SIZE=4096 -e CERVER_TH_THREADS=4 \
  -e CERVER_CONNECTION_QUEUE=4 \
  -e MONGO_APP_NAME=service -e MONGO_DB=worker \
  -e MONGO_URI=mongodb://service:password@192.168.100.39:27017/worker \
  -e CONNECT_TO_REDIS=true \
  -e REDIS_HOSTNAME=redis \
  ermiry/worker-service:development /bin/bash
```

## Routes

#### GET /api/service
**Description:** Worker service top level route \
**Returns:**
  - 200 on success

#### GET /api/service/version
**Access:** Public \
**Description:** Returns worker service current version \
**Returns:**
  - 200 and version's json on success

### State

#### GET /api/service/state
**Access:** Public \
**Description:** Returns uploads service workers states \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

#### GET /api/service/state/init
**Access:** Public \
**Description:** Initializes service workers state related values \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

#### GET /api/service/state/reset
**Access:** Public \
**Description:** Resets service workers state related values \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

### Data

#### GET /api/service/data
**Access:** Public \
**Description:** Returns uploads service data information \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

#### GET /api/service/data/init
**Access:** Public \
**Description:** Initializes cache data with default values \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

#### GET /api/service/data/reset
**Access:** Public \
**Description:** Admin has requested to reset service's data \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

#### GET /api/service/data/worker/queue
**Access:** Public \
**Description:** Returns an array of the trans in the worker queue \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

### Transactions

#### GET /api/transactions
**Access:** Public \
**Description:** Get all transactions \
**Returns:**
  - 200 and transactions json on success
  - 401 on failed auth

#### POST /api/transactions
**Access:** Public \
**Description:** A user has requested to create a new transaction \
**Returns:**
  - 200 on success creating transaction
  - 400 on failed to create new transaction
  - 401 on failed auth
  - 500 on server error

#### GET /api/transactions/:id/info
**Access:** Public \
**Description:** Returns information about an existing transaction \
**Returns:**
  - 200 and transaction's json on success
  - 401 on failed auth
  - 404 on transaction not found

#### PUT /api/transactions/:id/update
**Access:** Public \
**Description:** A user wants to update an existing transaction \
**Returns:**
  - 200 on success updating transaction
  - 400 on bad request due to missing values
  - 401 on failed auth
  - 500 on server error

#### DELETE /api/transactions/:id/remove
**Access:** Public \
**Description:** Deletes an existing transaction \
**Returns:**
  - 200 on success deleting transaction
  - 400 on bad request
  - 401 on failed auth
  - 500 on server error

### Worker

#### GET /api/worker/start
**Access:** Public \
**Description:** Requests to start the main worker \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error

#### GET /api/worker/stop
**Access:** Public \
**Description:** Requests to stop the main worker \
**Returns:**
  - 200 on success
  - 400 on bad user
  - 401 on failed auth
  - 500 on server error
