#include "include/crow.h"
#include "include/handlers.hpp"
#include "include/helpers.hpp"
#include <string>
#include <vector>

using namespace std;

#define PORT_NUMBER 3000

int main()
{
  crow::SimpleApp app;
  crow::mustache::set_base(".");

  DbHandler handler;

  CROW_ROUTE(app, "/api/pool")
    .methods("GET"_method)
    ([&](const crow::request &req) {
      crow::json::wvalue jResp;
      auto jBody = crow::json::load(req.body);

      if (jBody.error() ||
        !jBody.has("poolId") ||
        !jBody.has("percentile") ||
        crow::json::get_type_str(jBody["poolId"].t()) != "Number" ||
        crow::json::get_type_str(jBody["percentile"].t()) != "Number")
      {
        jResp["error"] = "Wrong json format received. Need {\"poolId\": number, \"percentile\": number(percentage)}";
        return crow::response(400, jResp);
      }

      int64_t poolId = jBody["poolId"].i();
      double percentile = jBody["percentile"].d();
      auto db = handler.get_connection(poolId);

      if (db->is_existed(poolId)) {
        jResp["total"] = db->get_total(poolId);
        jResp["quantile"] = quantileCalculate(db->get_values(poolId), percentile / 100);
      } else {
        jResp["error"] = "poolId has not found. Try to create one through POST(/api/pool)";
        return crow::response(410, jResp);
      }
      return crow::response(jResp);
    });


  CROW_ROUTE(app, "/api/pool")
    .methods("POST"_method)
    ([&](const crow::request &req) {
      crow::json::wvalue jResp;
      auto jBody = crow::json::load(req.body);

      if (jBody.error() ||
        !jBody.has("poolId") ||
        !jBody.has("poolValues") ||
        crow::json::get_type_str(jBody["poolId"].t()) != "Number" ||
        crow::json::get_type_str(jBody["poolValues"].t()) != "List")
      {
        jResp["error"] = "Wrong json format received. Need {\"poolId\": number, \"poolValues\": [num1, num2, ...]}";
        return crow::response(400, jResp);
      } else {
        for (auto value : jBody["poolValues"]) {
          if (crow::json::get_type_str(value.t()) != "Number") {
            jResp["error"] = "Wrong json format received. Need {\"poolId\": number, \"poolValues\": [num1, num2, ...]}";
            return crow::response(400, jResp);
          }
        }
      }

      int64_t poolId = jBody["poolId"].i();
      vector<int64_t> values;
      auto db = handler.get_connection(poolId);

      for (auto value : jBody["poolValues"]) {
        values.push_back(value.i());
      }
      jResp["status"] = getStatusStr(db->add(poolId, values));

      return crow::response(jResp);
    });

  app.port(PORT_NUMBER)
      .multithreaded()
      .run();
}
