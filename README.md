# PC-Pulse

Want to check how your computer is doing but your are far away?

## Folder Structure

```
server-pulse/
├── examples/                  # API request handlers and endpoints
├── metrics/                   # Gather the PC metrics
├── request/                   # Parse the requests
├── response/                  # Handle the responses
└── server/                    # building and managing the server

```

## 1. `/cpu` Endpoint

```json
{
  "cpu_usage_percent": float,
  "cpu_count": int,
  "cpu_model": string,
  "processes_count": int
}
```

## 2. `/memory` Endpoint

```json
{
  "total_mb": int,
  "used_mb": int,
  "available_mb": int,
  "usage_percent": float,
  "swap_total_mb": int,
  "swap_used_mb": int,
  "swap_usage_percent": float,
  "memory_pressure": string
}
```

## 3. `/disk` Endpoint

```json
{
  "total_disk": int,
  "free_disk": int,
  "used_disk": int,
  "disk_usage": float
}
```

## Reference

[RFC7230](https://datatracker.ietf.org/doc/html/rfc7230#section-2)
