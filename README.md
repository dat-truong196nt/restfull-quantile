# restfull-quantile


## Features:
- Micro web framework and json manipulation by [Crow](https://github.com/ipkn/crow).
- Json body validation.
- Sharding load balancer.
- Using insertion sort for nearly sorted data.
- Auto data disk storing in 5 minutes or 5000 requests.
- Simple database represented by files and map data structure on memory.

## Advantages:
- High-performance:
  - Data is mainly manipulated on memory.
  - Each request will be handled by a thread -> independent.
  - Using thread for storing data on disk -> Still able to handle incomming requests.
  - Sorting algorithm is faster for our special list.
- Resiliency:
  - Data is saved periodically on disk.
  - Data will be loaded on start up.
- High-availability:

- Scalability:
  - Have a load balancer for increasing requests.

## Usage:
### [GET] **/api/pool** --- Get percentile for pool list
- Body
  ```json
  {
    "poolId": Number,                 // id number
    "percentile": Number(Percentage)  // 0.00 -> 100.00
  }
  ```
- Respond
  - 200: Success
    ```json
    {
      "total": Number,    // Total value in list
      "quantile": Number, // Calculated quantile
    }
    ```
  - 400: Invalid json body
    ```json
    {
      "status": "Wrong json format received. Need {\"poolId\": number, \"percentile\": number(percentage)}"
    }
    ```
  - 401: Invalid poolId
    ```json
    {
      "status": "poolId has not found. Try to create one through POST(/api/pool)"
    }
    ```

### [POST] **/api/pool** --- Add values to pool list
- Body
  ```json
  {
    "poolId": Number,            // id number
    "poolValues": Array(Number)  // Ex: [2, 5, 1, 8]
  }
  ```
- Respond
  - 200: Success
    ```json
    {
      "status": "inserted/appended",  // Inserted for new id and appended for existed id
    }
    ```
  - 400: Invalid json body
    ```json
    {
      "status": "Wrong json format received. Need {\"poolId\": number, \"percentile\": number(percentage)}"
    }
    ```

## Build and Test:
### Build
```shell
make && build/rest_server
```

### Test
