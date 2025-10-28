#include "crow/app.h"
#include "crow/middlewares/cors.h"
#include "Trie.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace crow;

int main() {
    // Create our trie instance
    ::Trie trie;

    // Load dictionary
    std::ifstream infile("src/dictionary/words_alpha.txt");
    if (!infile.is_open()) {
        std::cerr << "Warning: Could not open dictionary file. Using empty dictionary.\n";
    } else {
        std::string word;
        int count = 0;
        while (getline(infile, word)) {
            trie.insert(word);
            count++;
        }
        std::cout << "Loaded " << count << " words from dictionary\n";
    }

    // Load persisted history
    trie.loadUserHistory("user_history.txt");
    std::cout << "Loaded user search history\n";

    // Create app with CORS middleware
    App<crow::CORSHandler> app;
    
    // Configure CORS
    auto& cors = app.get_middleware<crow::CORSHandler>();
    
    // Customize CORS
    cors
      .global()
        .headers("Content-Type", "Authorization")
        .methods("GET"_method, "POST"_method, "OPTIONS"_method);
        
    cors.prefix("/api")
       .origin("*");

    // Health check
    CROW_ROUTE(app, "/api/health")
    ([]() {
        crow::json::wvalue json_resp;
        json_resp["status"] = "ok";
        json_resp["message"] = "Server is running";
        
        crow::response res(json_resp);
        res.set_header("Content-Type", "application/json");
        return res;
    });

    // Suggest endpoint
    CROW_ROUTE(app, "/api/suggest")
    ([&trie](const crow::request& req) {
        auto prefix = req.url_params.get("prefix") ? req.url_params.get("prefix") : "";
        std::cout << "Suggestion request for prefix: '" << prefix << "'\n";
        
        auto suggestions = trie.autoCompleteSystem(prefix);
        
        crow::json::wvalue result;
        for (size_t i = 0; i < suggestions.size(); ++i)
            result["suggestions"][i] = suggestions[i];

        crow::response res(result);
        res.set_header("Content-Type", "application/json");
        std::cout << "Sent " << suggestions.size() << " suggestions\n";
        return res;
    });

    // Complete search endpoint
    CROW_ROUTE(app, "/api/search").methods("POST"_method)
    ([&trie](const crow::request& req) {
        std::cout << "Search request received\n";
        std::cout << "Request body: " << req.body << "\n";
        
        auto body = crow::json::load(req.body);
        if (!body || !body.has("query")) {
            crow::json::wvalue error_resp;
            error_resp["error"] = "Bad Request";
            error_resp["message"] = "Missing query field";
            
            crow::response res(400, error_resp);
            res.set_header("Content-Type", "application/json");
            return res;
        }

        std::string q = body["query"].s();
        std::cout << "Recording complete search: " << q << "\n";
        
        trie.recordCompleteSearch(q);
        trie.saveUserHistory("user_history.txt");

        crow::json::wvalue resp;
        resp["status"] = "success";
        resp["message"] = "Search recorded successfully";
        resp["query"] = q;
        
        crow::response res(resp);
        res.set_header("Content-Type", "application/json");
        return res;
    });

    // Add user word endpoint
    CROW_ROUTE(app, "/api/userword").methods("POST"_method)
    ([&trie](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("word")) {
            crow::json::wvalue error_resp;
            error_resp["error"] = "Bad Request";
            error_resp["message"] = "Missing word field";
            
            crow::response res(400, error_resp);
            res.set_header("Content-Type", "application/json");
            return res;
        }

        std::string w = body["word"].s();
        trie.insertUserWord(w);
        trie.saveUserHistory("user_history.txt");

        crow::json::wvalue resp;
        resp["status"] = "success";
        resp["message"] = "User word added successfully";
        resp["word"] = w;
        
        crow::response res(resp);
        res.set_header("Content-Type", "application/json");
        return res;
    });

    std::cout << "Starting server on port 8080...\n";
    std::cout << "API endpoints available:\n";
    std::cout << "  GET  /api/health\n";
    std::cout << "  GET  /api/suggest?prefix=<word>\n";
    std::cout << "  POST /api/search {\"query\": \"word\"}\n";
    std::cout << "  POST /api/userword {\"word\": \"word\"}\n";
    
    app.port(8080).multithreaded().run();
    return 0;
}