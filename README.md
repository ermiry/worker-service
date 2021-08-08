# Worker Test Service

### Development
```
sudo docker run \
  -it \
  --name worker --rm \
  -p 5010:5010 \
  -v /home/ermiry/Documents/ermiry/Projects/worker-service:/home/worker \
  -e RUNTIME=development \
  -e PORT=5010 \
  -e CERVER_RECEIVE_BUFFER_SIZE=4096 -e CERVER_TH_THREADS=4 \
  -e CERVER_CONNECTION_QUEUE=4 \
  ermiry/worker-service:development /bin/bash
```

## Routes

#### GET /api/worker
**Description:** Worker service top level route \
**Returns:**
  - 200 on success

#### GET /api/worker/version
**Access:** Public \
**Description:** Returns worker service current version \
**Returns:**
  - 200 and version's json on success
