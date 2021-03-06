//
// Created by gadi on 10/01/2020.
//

#ifndef EX4__MYCLIENTHANDLER_H_
#define EX4__MYCLIENTHANDLER_H_

#include "DepthFirstSearch.h"
#include "BestFirstSearch.h"
#include "ClientHandler.h"
#include "Solver.h"
#include "CacheManager.h"
#include "Matrix.h"

/**
 * class that handle client and get the msg from him. check if the solution exist in the cache, if not
 * solve the problem and return the solution and close the socket
 * @tparam Problem
 * @tparam Solution
 */
template<class Problem, class Solution>
class MyClientHandler : public ClientHandler {
 private:
  SolverToSearcherAdapter<std::string, Searchable<Point> *, Point> *adapter;
  CacheManager<Problem, Solution> *cache_manager_;

 public:
  /**
   * CTOR
   */
  MyClientHandler() {
    auto star = new AStarSearch<std::string, Point>();
    this->adapter = new SolverToSearcherAdapter<std::string, Searchable<Point> *, Point>(star);
    this->cache_manager_ = new FileCacheManager<Problem, Solution>(100);
  }/**
 * another CTOR
 * @param ad adapter solver
 * @param cache this cache ,the cache is common cahce in this program
 */
  MyClientHandler(SolverToSearcherAdapter<std::string, Searchable<Point> *, Point> *ad,
                  CacheManager<Problem, Solution> *cache) : adapter(ad), cache_manager_(cache) {}
  /**
   * DTOR
   */
  ~MyClientHandler() {
    delete this->cache_manager_;
    delete this->adapter;
  }
  /**
   * clone method that make copy for the client handler for parallel server.
   * @return
   */
  MyClientHandler *clone() override {
    SolverToSearcherAdapter<std::string, Searchable<Point> *, Point> *adp = adapter->clone();
    MyClientHandler *handler = new MyClientHandler<std::string, std::string>(adp, this->cache_manager_);
    return handler;
  }
  /**
   * read by buffer the msg from the client ,after that, check if there is solution in the cache
   * if there is solution in chche the handler return it ,else sent to solve it by the member adapter.
   * @param client_socket
   * @param server_socket
   */
  void handleClient(int client_socket, int server_socket) override {
    list<std::string> matrix;
    char buffer[2048] = {0};
    string msg;
    string check;
    int size = 0;
    read(client_socket, buffer, 2048);
    while (!std::strstr(buffer, "end")) {
      size++;
      msg = buffer;
      check += msg;
      matrix.emplace_back(msg);
      std::fill(std::begin(buffer), std::end(buffer), 0);
      read(client_socket, buffer, 1024);
    }
    msg = buffer;
    check += msg;
    Matrix *my_matrix = MatrixBuilder::parser_the_message(check);
    /**
     * check if exist in cache ,return solution else ,solve the problem
     * need to check if exist the problem
     */
    Solution sol;
    if (this->cache_manager_->isExist(check)) {
      cout << "from cache" << endl;
      sol = this->cache_manager_->get(check);
    } else {
      cout << "from solver" << endl;
      sol = this->adapter->solve(my_matrix);
      this->cache_manager_->insert(check, sol);
    }
    cout << sol << endl;
    const char *to_send;
    to_send = sol.c_str();
    send(client_socket, to_send, strlen(to_send), 0);
    cout << "close socket-finish" << endl;
    close(client_socket);
  }
};
#endif //EX4__MYCLIENTHANDLER_H_
