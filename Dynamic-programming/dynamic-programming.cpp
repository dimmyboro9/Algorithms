#ifndef __PROGTEST__
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>

using ChristmasTree = size_t;

struct TreeProblem {
  int max_group_size;
  std::vector<uint64_t> gifts;
  std::vector<std::pair<ChristmasTree, ChristmasTree>> connections;
};

#endif

using namespace std;

struct Sone
{
         Sone ( ) : m_W ( 0 ), m_Wo ( 0 ) { };
  size_t m_W;
  size_t m_Wo;
};

struct Stwo
{
         Stwo ( ) : m_Dad ( 0 ), m_Son ( 0 ), m_Wo ( 0 ), m_Best_Son ( -1 ) { };
  size_t m_Dad;
  size_t m_Son;
  size_t m_Wo;
  int    m_Best_Son;
};

class CSolve 
{
  public:
                                                CSolve ( const TreeProblem& bodyguards ) : m_Neighbours ( bodyguards . gifts . size ( ) ), m_Gifts ( bodyguards . gifts ) { add_neighbours ( bodyguards . connections ); };
    size_t                                      one_guard ( size_t trees_num );
    size_t                                      two_guards ( size_t trees_num );
  private:
    void                                        add_neighbours ( const vector < pair < ChristmasTree, ChristmasTree > >& connections );
    void                                        is_list ( size_t size, size_t tree, unordered_set < ChristmasTree >& lists ) const;
    void                                        add_son ( vector < Stwo >& solver, size_t neighbour, size_t tree ) const;
    
    vector < unordered_set < ChristmasTree > >  m_Neighbours;
    vector < uint64_t >                         m_Gifts;
    queue < ChristmasTree >                     m_Queue;
};

void CSolve:: is_list ( size_t size, size_t tree, unordered_set < ChristmasTree >& lists ) const
{
  if ( size == 1 )
    lists . insert ( tree );
  else if ( size == 2 )
    lists . erase ( tree );
}

void CSolve:: add_neighbours ( const vector < pair < ChristmasTree, ChristmasTree > >& connections )
{
  unordered_set < ChristmasTree > leaves;
  for ( auto connection : connections )
  {
    m_Neighbours [ connection . first ] . insert ( connection . second );
    is_list ( m_Neighbours [ connection . first ] . size ( ), connection . first, leaves );
    m_Neighbours [ connection . second ] . insert ( connection . first );
    is_list ( m_Neighbours [ connection . second ] . size ( ), connection . second, leaves );
  }
  for ( auto leaf : leaves )
    m_Queue . push ( leaf );
}

void CSolve:: add_son ( vector < Stwo >& solver, size_t neighbour, size_t tree ) const
{
  if ( solver [ solver [ neighbour ] . m_Best_Son ] . m_Dad + solver [ tree ] . m_Wo <= solver [ solver [ neighbour ] . m_Best_Son ] . m_Wo + solver [ tree ] . m_Dad )
  {
    solver [ neighbour ] . m_Son += ( solver [ solver [ neighbour ] . m_Best_Son ] . m_Wo + solver [ tree ] . m_Dad - solver [ solver [ neighbour ] . m_Best_Son ] . m_Dad );
    solver [ neighbour ] . m_Best_Son = tree;
  }
  else
    solver [ neighbour ] . m_Son += solver [ tree ] . m_Wo;
}

size_t CSolve:: one_guard ( size_t trees_num )
{
  vector < Sone > solver ( trees_num );
  size_t num = 0;
  while ( m_Queue . size ( ) )
  {
    ChristmasTree tree = m_Queue . front ( );
    solver [ tree ] . m_W += m_Gifts [ tree ];
    if ( m_Neighbours [ tree ] . size ( ) )
    {
      size_t neighbour = *m_Neighbours [ tree ] . begin ( );
      solver [ neighbour ] . m_W += solver [ tree ] . m_Wo;
      solver [ neighbour ] . m_Wo += max ( solver [ tree ] . m_Wo, solver [ tree ] . m_W );
      m_Neighbours [ neighbour ] . erase ( tree );
      if ( m_Neighbours [ neighbour ] . size ( ) == 1 )
        m_Queue . push ( neighbour );
    }
    if ( m_Queue . size ( ) == 1 )
      num = max ( solver [ tree ] . m_W, solver [ tree ] . m_Wo );
    m_Queue . pop ( );
  }
  return num;
}

size_t CSolve:: two_guards ( size_t trees_num )
{
  vector < Stwo > solver ( trees_num );
  size_t num = 0;
  while ( m_Queue . size ( ) )
  {
    ChristmasTree tree = m_Queue . front ( );
    solver [ tree ] . m_Dad += m_Gifts [ tree ];
    solver [ tree ] . m_Son += m_Gifts [ tree ];
    if ( m_Neighbours [ tree ] . size ( ) )
    {
      size_t neighbour = *m_Neighbours [ tree ] . begin ( );
      solver [ neighbour ] . m_Wo += max ( solver [ tree ] . m_Wo, solver [ tree ] . m_Son ); // probably without max
      solver [ neighbour ] . m_Dad += solver [ tree ] . m_Wo;
      if ( solver [ tree ] . m_Wo >= solver [ tree ] . m_Dad )
        solver [ neighbour ] . m_Son += solver [ tree ] . m_Wo;
      else if ( solver [ neighbour ] . m_Best_Son < 0 )
      {
        solver [ neighbour ] . m_Best_Son = tree;
        solver [ neighbour ] . m_Son += solver [ tree ] . m_Dad; 
      }
      else
        add_son ( solver, neighbour, tree );
      m_Neighbours [ neighbour ] . erase ( tree );
      if ( m_Neighbours [ neighbour ] . size ( ) == 1 )
        m_Queue . push ( neighbour );
    }
    if ( m_Queue . size ( ) == 1 )
      num = max ( solver [ tree ] . m_Son, solver [ tree ] . m_Wo );
      // num = max ( num, solver [ tree ] . m_Dad );
    m_Queue . pop ( );
  }
  return num;
}

uint64_t solve( const TreeProblem& bodyguards ) 
{
  CSolve data ( bodyguards );
  if ( bodyguards . max_group_size == 1 )
    return data . one_guard ( bodyguards . gifts . size ( ) );
  return data . two_guards ( bodyguards . gifts . size ( ) );  
}

#ifndef __PROGTEST__

using TestCase = std::pair<uint64_t, TreeProblem>;

const std::vector<TestCase> BASIC_TESTS = {
  { 3, { 1, { 1, 1, 1, 2 }, { {0,3}, {1,3}, {2,3} }}},
  { 4, { 1, { 1, 1, 1, 4 }, { {0,3}, {1,3}, {2,3} }}},
  { 57, { 1, {
    17, 11, 5, 13, 8, 12, 7, 4, 2, 8,
  }, {
    {1, 4}, {6, 1}, {2, 1}, {3, 8}, {8, 0}, {6, 0}, {5, 6}, {7, 2}, {0, 9},
  }}},
  { 85, { 1, {
    10, 16, 13, 4, 19, 8, 18, 17, 18, 19, 10,
  }, {
    {9, 7}, {9, 6}, {10, 4}, {4, 9}, {7, 1}, {0, 2}, {9, 2}, {3, 8}, {2, 3}, {5, 4},
  }}},
  { 79, { 1, {
    8, 14, 11, 8, 1, 13, 9, 14, 15, 12, 1, 11,
  }, {
    {9, 1}, {1, 2}, {1, 4}, {5, 10}, {7, 8}, {3, 7}, {11, 3}, {11, 10}, {6, 8}, {0, 1}, {0, 3},
  }}},
  { 102, { 1, {
    15, 10, 18, 18, 3, 4, 18, 12, 6, 19, 9, 19, 10,
  }, {
    {10, 2}, {11, 10}, {6, 3}, {10, 8}, {5, 3}, {11, 1}, {9, 5}, {0, 4}, {12, 3}, {9, 7}, {11, 9}, {4, 12},
  }}},
  { 93, { 1, {
    1, 7, 6, 18, 15, 2, 14, 15, 18, 8, 15, 1, 5, 6,
  }, {
    {0, 13}, {6, 12}, {0, 12}, {7, 8}, {8, 3}, {12, 11}, {12, 1}, {10, 12}, {2, 6}, {6, 9}, {12, 7}, {0, 4}, {0, 5},
  }}},
};

const std::vector<TestCase> BONUS_TESTS = {
  { 3, { 2, { 1, 1, 1, 2 }, { {0,3}, {1,3}, {2,3} }}},
  { 5, { 2, { 1, 1, 1, 4 }, { {0,3}, {1,3}, {2,3} }}},
};

void test(const std::vector<TestCase>& T) {
  int i = 0;
  for (auto &[s, t] : T) {
    if (s != solve(t))
      std::cout << "Error in " << i << " (returned " << solve(t) << ")"<< std::endl;
    i++;
  }
  std::cout << "Finished" << std::endl;
}

int main() {
  test(BASIC_TESTS);
  test(BONUS_TESTS);
}

#endif


