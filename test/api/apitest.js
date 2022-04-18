import fetch from 'node-fetch'

const PORT = 3000
const URL = "localhost"
const ENDPOINT_PREFIX = `http://${URL}:${PORT}/api`

const requestPost = (data, endpoint) => fetch(`${ENDPOINT_PREFIX}${endpoint}`, {
  method: "POST",
  body: JSON.stringify(data),
});

const api_check_add_schema = async () => {
  const wrongSchemas = [{
    poolId: 123,
  }, {
    poolValues: [1, 2, 3],
  }, {
    poolId: [1, 2, 3],
    poolValues: [1, 2, 3],
  }, {
    poolId: {a: {}, b: {}, c: {}},
    poolValues: [1, 2, 3],
  }, {
    poolId: "string",
    poolValues: [1, 2, 3],
  }, {
    poolId: true,
    poolValues: [1, 2, 3],
  }, {
    poolId: 123,
    poolValues: 123,
  }, {
    poolId: 123,
    poolValues: {a: {}, b: {}, c: {}},
  }, {
    poolId: 123,
    poolValues: "string",
  }, {
    poolId: 123,
    poolValues: true,
  },{}];

  wrongSchemas.map(async (schema) => {
    try {
      const resp = await requestPost(schema, '/add');
      return false;
    } catch (err) {
      // Passed
    }
  })

  return true;
}

const api_check_add = async () => {
  const reqBody = { poolId: 9999, poolValues: [8, 2, 3] }

  try {
    const resp = await requestPost(reqBody, '/add');
    const body = await resp.json();
    if (body.status !== 'appended' && body.status !== 'inserted') return false;
    // Passed
  } catch (err) {
    console.log(err)
    return false;
  }

  return true;
}

const api_check_get_quantile_schema = async () => {
  const wrongSchemas = [{
    poolId: 123,
  }, {
    percentile: 50.5,
  }, {
    poolId: [1, 2, 3],
    percentile: 50.5,
  }, {
    poolId: {a: {}, b: {}, c: {}},
    percentile: 50.5,
  }, {
    poolId: "string",
    percentile: 50.5,
  }, {
    poolId: 50.5,
    percentile: 50.5,
  }, {
    poolId: true,
    percentile: 50.5,
  }, {
    poolId: 123,
    percentile: [1, 2, 3],
  }, {
    poolId: 123,
    percentile: {a: {}, b: {}, c: {}},
  }, {
    poolId: 123,
    percentile: "string",
  }, {
    poolId: 123,
    percentile: true,
  },{}];

  wrongSchemas.map(async (schema) => {
    try {
      const resp = await requestPost(schema, '/quantile');
      return false;
    } catch (err) {
      // Passed
    }
  })

  return true;
}

const api_check_get_quantile = async () => {
  const reqBody = { poolId: 9999, percentile: 50.5 };

  try {
    const resp = await requestPost(reqBody, '/quantile');
    const body = await resp.json();
    if (!body.quantile || !body.total) return false;
    // Passed
  } catch (err) {
    return false;
  }

  return true;
}

// main
let passed = 0, total = 0;

total++;
if(await api_check_add_schema() !== true) {
  console.error('Failed: api_check_add_schema');
} else {
  console.debug('Passed: api_check_add_schema');
  passed++;
}

total++;
if(await api_check_get_quantile_schema() !== true) {
  console.error('Failed: api_check_get_quantile_schema');
} else {
  console.debug('Passed: api_check_get_quantile_schema');
  passed++;
}

total++;
if(await api_check_add() !== true) {
  console.error('Failed: api_check_add');
} else {
  console.debug('Passed: api_check_add');
  passed++;
}

total++;
if(await api_check_get_quantile() !== true) {
  console.error('Failed: api_check_get_quantile');
} else {
  console.debug('Passed: api_check_get_quantile');
  passed++;
}

console.log(`API Testing: passed ${passed}/${total} test cases.`);
console.log();
