# PC-Pulse

A HTTP server that can handle requests and generate responses concurrently. It serves the metrics about the system.

## Features

- [x] Parse the requests
- [x] generates the responses
- [x] can add routes
- [x] can add tasks to the task queue
- [x] can use the threads from the thread pool to handle the requests concurrently
- [x] Linux based system Support(for metrics)
- [ ] Windows Support

## Folder Structure

```
server-pulse/
├── examples/                  # API request handlers and endpoints
├── metrics/                   # Gather the PC metrics
├── request/                   # Parse the requests
├── response/                  # Handle the responses
├── route/                     # Handle the routes
├── task_queue/                # Handle the task queue
└── thread_pool/               # Handle the thread pool
└── server/                    # building and managing the server

```

## 1. `/cpu` Endpoint

```json

  "cpu_usage_percent": float,
  "cpu_count": int,
  "cpu_model": string,
  "processes_count": int
```

## 2. `/memory` Endpoint

```json

  "total_memory": float,
  "free_memory": float,
  "cached_memory": float,
  "swap_total": float,
  "swap_free": float,
  "memory_pressure": float

```

## 3. `/disk` Endpoint

```json

  "total_disk": float,
  "free_disk": float,
  "used_disk": float,
  "disk_usage": float

```

## Reference

[RFC7230](https://datatracker.ietf.org/doc/html/rfc7230#section-2)
