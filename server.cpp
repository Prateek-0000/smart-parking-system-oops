#include "crow.h"
#include "crow/middlewares/cors.h" // <-- Required include
#include <string>
#include <vector>
#include <iostream>

// --- OOP Model ---

class ParkingSpot {
public:
    int spotNumber;
    bool isOccupied;
    std::string vehicleNumber;
    std::string vehicleType;

    ParkingSpot(int num)
        : spotNumber(num), isOccupied(false), vehicleNumber(""), vehicleType("") {}

    void occupy(const std::string& num, const std::string& type) {
        isOccupied = true;
        vehicleNumber = num;
        vehicleType = type;
    }

    void vacate() {
        isOccupied = false;
        vehicleNumber = "";
        vehicleType = "";
    }
};

class ParkingLot {
private:
    std::vector<ParkingSpot> spots;
    int totalSpots;

public:
    ParkingLot(int size) : totalSpots(size) {
        for (int i = 1; i <= size; ++i) {
            spots.push_back(ParkingSpot(i));
        }
    }

    std::string parkVehicle(const std::string& number, const std::string& type) {
        for (const auto& spot : spots) {
            if (spot.isOccupied && spot.vehicleNumber == number) {
                return "Vehicle " + number + " is already parked.";
            }
        }
        for (auto& spot : spots) {
            if (!spot.isOccupied) {
                spot.occupy(number, type);
                return "Vehicle " + number + " parked successfully at Spot " + std::to_string(spot.spotNumber) + ".";
            }
        }
        return "Sorry, the parking lot is full.";
    }

    std::string exitVehicle(const std::string& number) {
        for (auto& spot : spots) {
            if (spot.isOccupied && spot.vehicleNumber == number) {
                int spotNum = spot.spotNumber;
                spot.vacate();
                return "Vehicle " + number + " exited from Spot " + std::to_string(spotNum) + ".";
            }
        }
        return "Vehicle " + number + " not found in the parking lot.";
    }

    const std::vector<ParkingSpot>& getSpots() const {
        return spots;
    }
};

// --- Main Server Logic ---

int main() {
    crow::App<crow::CORSHandler> app;

    // Use get_middleware (not middleware)
    app.get_middleware<crow::CORSHandler>()
        .global()
        .headers("Content-Type")
        .methods(crow::HTTPMethod::POST, crow::HTTPMethod::GET);

    ParkingLot myLot(10);

    CROW_ROUTE(app, "/status").methods(crow::HTTPMethod::GET)
    ([&myLot]() {
        std::vector<crow::json::wvalue> spotList;
        for (const auto& spot : myLot.getSpots()) {
            crow::json::wvalue s;
            s["spot"] = spot.spotNumber;
            s["status"] = spot.isOccupied ? "OCCUPIED" : "FREE";
            s["vehicleNumber"] = spot.isOccupied ? spot.vehicleNumber : crow::json::wvalue(nullptr);
            s["vehicleType"] = spot.isOccupied ? spot.vehicleType : crow::json::wvalue(nullptr);
            spotList.push_back(std::move(s));
        }
        return crow::json::wvalue(spotList);
    });

    CROW_ROUTE(app, "/park").methods(crow::HTTPMethod::POST)
    ([&myLot](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string number = body["number"].s();
        std::string type = body["type"].s();
        std::string message = myLot.parkVehicle(number, type);

        crow::json::wvalue res_json;
        res_json["message"] = message;

        if (message.find("successfully") != std::string::npos) {
            return crow::response(200, res_json);
        } else {
            return crow::response(400, res_json);
        }
    });

    CROW_ROUTE(app, "/exit").methods(crow::HTTPMethod::POST)
    ([&myLot](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string number = body["number"].s();
        std::string message = myLot.exitVehicle(number);

        crow::json::wvalue res_json;
        res_json["message"] = message;

        if (message.find("exited") != std::string::npos) {
            return crow::response(200, res_json);
        } else {
            return crow::response(404, res_json);
        }
    });

    std::cout << "Starting C++ Smart Parking server on port 3000..." << std::endl;
    app.port(3000).run();
    return 0;
}