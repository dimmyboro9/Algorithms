#ifndef __PROGTEST__
#include <cassert>
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

using Place = size_t;

struct Map {
  size_t places;
  Place start, end;
  std::vector<std::pair<Place, Place>> connections;
  std::vector<std::vector<Place>> items;
};

template < typename F, typename S >
struct std::hash<std::pair<F, S>> {
  std::size_t operator () (const std::pair<F, S> &p) const noexcept {
    // something like boost::combine would be much better
    return std::hash<F>()(p.first) ^ (std::hash<S>()(p.second) << 1);
  }
};

#endif

using namespace std;

class CRoom
{
  public:
                                                                                CRoom ( Place place, shared_ptr < CRoom > pred ) : m_Place ( place ), m_Predcessor ( pred ) { };
    void                                                                        add_to_list ( list < Place >& to_return ) const;
  private:
    Place                                                                       m_Place;
    shared_ptr < CRoom >                                                        m_Predcessor;
};

void CRoom:: add_to_list ( list < Place >& to_return ) const
{
  to_return . push_front ( m_Place );
  if ( m_Predcessor != nullptr )
    m_Predcessor -> add_to_list ( to_return );
  return;
}

unordered_map < Place, vector < Place > > add_connections ( const vector < pair < Place, Place > >& connections )
{
  unordered_map < Place, vector < Place > > neighbours;
  for ( auto i : connections )
  {
    neighbours [ i . first ] . emplace_back ( i . second );
    neighbours [ i . second ] . emplace_back ( i . first );
  }
  return neighbours;
}

tuple < unordered_map < Place, uint16_t >, uint16_t, bool > add_items ( const vector < vector < Place > >& items )
{
  unordered_map < Place, uint16_t > items_map;
  size_t num = 0;
  uint16_t all = 0;
  for ( auto i : items )
  {
    uint16_t bit_mask = 1;
    if ( ! i . size ( ) )
      return make_tuple ( items_map, 0, false );
    for ( auto item : i )
      if ( items_map . find ( item ) == items_map . end ( ) )
        items_map [ item ] = bit_mask << num;
      else
        items_map [ item ] |= bit_mask << num;
    num++;
  }
  for ( size_t n = 0; n < num; n++ )
    if ( n == 0 )
      all = 1;
    else
      all |= all << 1;
  return make_tuple ( items_map, all, true );
}

uint16_t how_many_items ( Place pos, uint16_t amount, const unordered_map < Place, uint16_t >& items )
{
  if ( items . find ( pos ) == items . end ( ) )
    return amount;
  return amount | items . at ( pos ); 
}

void delete_repeats ( const unordered_map < uint16_t, shared_ptr < CRoom > >& items_in_prev_appear, unordered_map < uint16_t, uint16_t >& prev_items )
{
  for ( auto i = prev_items . begin ( ); i != prev_items . end ( ); )
  {
    bool is_erased = false;
    for ( auto j : items_in_prev_appear )
      if ( ( *i ) . second == ( ( *i ) . second & j . first ) )
      {
        auto to_del = i;
        i++;
        prev_items . erase ( to_del );
        is_erased = true;
        break;
      }
    if ( ! is_erased )
      i++;
  }
}

list < Place > make_list ( const shared_ptr < CRoom >& end )
{
  list < Place > to_return;
  end -> add_to_list ( to_return ); 
  return to_return;
}

list < Place > create_graph ( Place st, Place en, const unordered_map < Place, vector < Place > >& neighbours, const unordered_map < Place, uint16_t >& items, uint16_t all )
{
  queue < pair < Place, size_t > > to_visit;
  unordered_map < pair < Place, size_t >, unordered_map < uint16_t, shared_ptr < CRoom > >, hash < pair < Place, size_t > > > is_visited;
  unordered_map < Place, vector < size_t > > appearances;
  to_visit . emplace ( st, 0 );
  uint16_t start_items = how_many_items ( st, 0, items );
  is_visited [ make_pair ( st, 0 ) ] . emplace ( start_items, make_shared < CRoom > ( st, nullptr ) );
  if ( st == en && start_items == all )
    return make_list ( is_visited . at ( make_pair ( st, 0 ) ) . at ( start_items ) );
  appearances [ st ] . emplace_back ( 0 );
  while ( to_visit . size ( ) )
  {
    pair < Place, size_t > pred = to_visit . front ( );
    if ( neighbours . find ( pred . first ) != neighbours . end ( ) )
      for ( auto neighbour : neighbours . at ( pred . first ) )
      {
        unordered_map < uint16_t, uint16_t > prev_items;
        for ( auto i : is_visited . at ( pred ) )
          prev_items . emplace ( i . first, how_many_items ( neighbour, i . first, items ) );
        if ( appearances . find ( neighbour ) != appearances . end ( ) )
        {
          if ( appearances . at ( neighbour ) . back ( ) == pred . second + 1 )
          {
            if ( appearances . at ( neighbour ) . size ( ) > 1 )
              delete_repeats ( is_visited . at ( make_pair ( neighbour, appearances . at ( neighbour ) . at ( appearances . at ( neighbour ) . size ( ) - 2 ) ) ), prev_items );
          }
          else
            delete_repeats ( is_visited . at ( make_pair ( neighbour, appearances . at ( neighbour ) . at ( appearances . at ( neighbour ) . size ( ) - 1 ) ) ), prev_items );
        }
        if ( prev_items . size ( ) )
        {
          if ( is_visited . find ( make_pair ( neighbour, pred . second + 1 ) ) == is_visited . end ( ) )
          {
            to_visit . emplace ( neighbour, pred . second + 1 );
            appearances [ neighbour ] . emplace_back ( pred . second + 1 );
            for ( auto new_item : prev_items )
            {
              is_visited [ make_pair ( neighbour, pred . second + 1 ) ] . emplace ( new_item . second, make_shared < CRoom > ( neighbour, is_visited [ pred ] . at ( new_item . first ) ) );
              if ( neighbour == en && new_item . second == all )
                return make_list ( is_visited . at ( make_pair ( neighbour, pred . second + 1 ) ) . at ( new_item . second ) );
            }
          }
          else
            for ( auto new_item : prev_items )
            {
              is_visited . at ( make_pair ( neighbour, pred . second + 1 ) ) . emplace ( new_item . second, make_shared < CRoom > ( neighbour, is_visited . at ( pred ) . at ( new_item . first ) ) );
              if ( neighbour == en && new_item . second == all )
                return make_list ( is_visited . at ( make_pair ( neighbour, pred . second + 1 ) ) . at ( new_item . second ) );
            }
        }
      }
    to_visit . pop ( );
  }
  list < Place > empty_list;
  return empty_list;
}

std::list<Place> find_path(const Map &map) 
{
  list < Place > path;
  unordered_map < Place, vector < Place > > neighbours = add_connections ( map . connections );
  tuple < unordered_map < Place, uint16_t >, uint16_t, bool > items = add_items ( map . items );
  if ( get < 2 > ( items ) )
    path = create_graph ( map . start, map . end, neighbours, get < 0 > ( items ), get < 1 > ( items ) );
  return path;
}

#ifndef __PROGTEST__

using TestCase = std::pair<size_t, Map>;

// Class template argument deduction exists since C++17 :-)
const std::array examples = {
  TestCase{ 1, Map{ 2, 0, 0,
    { { 0, 1 } },
    { { 0 } }
  }},
  TestCase{ 3, Map{ 2, 0, 0,
    { { 0, 1 } },
    { { 1 } }
  }},
  TestCase{ 3, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    {}
  }},
  TestCase{ 4, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 } }
  }},
  TestCase{ 0, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 }, {} }
  }},
  TestCase{ 0, Map{ 8, 0, 7,
    { { 0, 7 }, { 2, 7 } },
    { { 0 }, { 1, 4 }, { 2, 7 }, { 1, 2 } }
  }},
  TestCase{ 6, Map{ 8, 0, 7,
    { { 0, 7 }, { 2, 7 }, { 2, 1 } },
    { { 0 }, { 1, 4 }, { 2, 7 }, { 1, 2 } }
  }},
  TestCase{ 8, Map{ 10, 0, 8,
    { { 0, 1 }, { 1, 3 }, { 1, 2 }, { 1, 5 }, { 4, 5 }, { 4, 6 }, { 6, 7 }, { 7, 8 }, { 5, 8 }, { 5, 9 }, { 4, 9 }, { 2, 4 }  },
    { { 1, 4 }, { 9 }, { 3, 7 } }
  }},
};

int main() {
  int fail = 0;
  for (size_t i = 0; i < examples.size(); i++) {
    auto sol = find_path(examples[i].second);
    if (sol.size() != examples[i].first) {
      std::cout << "Wrong anwer for map " << i << std::endl;
      fail++;
    }
  }

  if (fail) std::cout << "Failed " << fail << " tests" << std::endl;
  else std::cout << "All tests completed" << std::endl;

  return 0;
}

#endif
