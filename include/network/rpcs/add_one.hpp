// #pragma once
// #include "network/rpc.hpp"
// int a{0};

// network::RPC<network::Datagram<>, [](const network::Datagram<> &data)
//              {a++;std::cout << a << "\n"; }>
//     add_one;
// network::RPC<network::Datagram<int>, [](const network::Datagram<int> &data)
//              {
//     std::cout << data.get<0>() << "\n";
//     a = data.get<0>(); }>
//     set;

// network::RPC<network::Datagram<std::vector<int>>, [](const network::Datagram<std::vector<int>> &data)
//              {
//                  std::cout << "vector:\n";

//                  int i{0};
//                  for (const auto &number : data.get<0>())
//                  {
//                     if(i++ != number)
//                      std::cout << i << ": " << number << ", ";
//                  }
//                  std::cout << "\n";
//              }>
//     vector;



// network::RPC<network::Datagram<std::array<int,3>>, [](const network::Datagram<std::array<int,3>> &data)
//              {
//                  std::cout << "array:\n";

//                  int i{0};
//                  for (const auto &number : data.get<0>())
//                  {
//                     if(i++ != number)
//                      std::cout << i << ": " << number << ", ";
//                  }
//                  std::cout << "\n";
//              }>
//     array;