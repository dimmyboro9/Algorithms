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
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>

#endif

using namespace std;

size_t max ( size_t a, size_t b )
{
  return a >= b ? a : b;
}

template < typename Product >
const Product& find_in_set ( const unordered_set < Product >& container, size_t id )
{
  auto it = container . begin ( );
  for ( size_t i = 0; i < id; i++ )
    it++;
  return *it;
}

template < typename Product >
size_t find_in_set ( const unordered_set < Product >& container, const Product& p )
{
  size_t id = 0;
  for ( auto prod : container )
    if ( prod == p )
      break;
    else
      id++;
  return id;
}

template < typename Product >
struct Bestsellers 
{
  public:
    // The total number of tracked products
    size_t                                                                products () const { return m_Products . size ( ); };

    void                                                                  sell(const Product& p, size_t amount);

    // The most sold product has rank 1
    size_t                                                                rank(const Product& p) const;
    const Product&                                                        product(size_t rank) const;

    // How many copies of product with given rank were sold
    size_t                                                                sold(size_t rank) const;
    // The same but sum over interval of products (including from and to)
    // It must hold: sold(x) == sold(x, x)
    size_t                                                                sold(size_t from, size_t to) const;

    // Bonus only, ignore if you are not interested in bonus
    // The smallest (resp. largest) rank with sold(rank) == sold(r)
    size_t                                                                first_same(size_t r) const;
    size_t                                                                last_same(size_t r) const;
  
private:
    class CTree
    {
      private:
        struct CNode
        {
                                                          CNode             ( const Product& p, size_t amount, shared_ptr < CNode > dad ) : m_Amount ( amount ), m_Same_Am_Prod ( { Product ( p ) } ), m_Height ( 0 ), m_Sons_Am ( 0 ), m_Sons_Sum ( 0 ), m_Dad ( dad ) { };

          size_t                                          m_Amount;
          unordered_set < Product >                       m_Same_Am_Prod;
          size_t                                          m_Height;
          size_t                                          m_Sons_Am;
          size_t                                          m_Sons_Sum;

          shared_ptr < CNode >                            m_Dad;
          shared_ptr < CNode >                            m_L;
          shared_ptr < CNode >                            m_R;
        };                  
        shared_ptr < CNode >                              m_Root;
        struct CRank
        {
                                                          CRank ( size_t amount, size_t sum, size_t id, size_t size ) : m_Amount ( amount ), m_Sum ( sum ), m_ID ( id ), m_Size ( size ) { };
          size_t                                          m_Amount;
          size_t                                          m_Sum;
          size_t                                          m_ID;
          size_t                                          m_Size;
        };

        void                                              set_null_on_dad   ( shared_ptr < CNode >& node );
        void                                              insert            ( const Product& p, size_t amount, shared_ptr < CNode >& node, shared_ptr < CNode > dad );
        void                                              remove            ( const Product& p, size_t amount ) { remove ( p, amount, m_Root, 1 ); };
        void                                              remove            ( const Product& p, size_t amount, shared_ptr < CNode >& node, size_t size );
        void                                              left_balance      ( shared_ptr < CNode >& node );      
        void                                              right_balance     ( shared_ptr < CNode >& node );
        void                                              l_rotation        ( shared_ptr < CNode >& node );
        void                                              r_rotation        ( shared_ptr < CNode >& node );
        size_t                                            find_product         ( const Product& p, size_t amount, shared_ptr < CNode > node ) const;

      public: 
                                                          CTree             ( ) { };
                                                         ~CTree             ( ) { set_null_on_dad ( m_Root ); };
        void                                              insert            ( const Product& p, size_t amount ) { insert ( p, amount, m_Root, nullptr ); };
        void                                              change_amount     ( const Product& p, size_t prev_amount, size_t new_amount );
        size_t                                            find_product      ( const Product& p, size_t amount ) const { return find_product ( p, amount, m_Root ); };
        const Product&                                    find_rth_product  ( size_t rank, size_t quantity ) const;
        CRank                                             find_rth_amount  ( size_t rank, size_t quantity ) const
        {
          size_t id = quantity - rank, sum = 0;
          auto node = m_Root;
          while ( true )
          {
            if ( ! node -> m_L || node -> m_L -> m_Same_Am_Prod . size ( ) + node -> m_L -> m_Sons_Am <= id )
            {
              id -= ( node -> m_L ? node -> m_L -> m_Same_Am_Prod . size ( ) + node -> m_L -> m_Sons_Am : 0 );
              sum += ( node -> m_L ? node -> m_L -> m_Same_Am_Prod . size ( ) * node -> m_L -> m_Amount + node -> m_L -> m_Sons_Sum : 0 );
              if ( node -> m_Same_Am_Prod . size ( ) <= id )
              {
                id -= node -> m_Same_Am_Prod . size ( );
                sum += node -> m_Same_Am_Prod . size ( ) * node -> m_Amount;
                node = node -> m_R;
              }
              else
                return CRank ( node -> m_Amount, sum += node -> m_Amount * ( id + 1 ), node -> m_Same_Am_Prod . size ( ) - 1 - id, node -> m_Same_Am_Prod . size ( ) );
            }
            // else if ( id < node -> m_L -> m_Same_Am_Prod . size ( ) )
            //   return make_pair ( find_in_set ( node -> m_L -> m_Same_Am_Prod, id ), make_pair ( node -> m_L -> m_Amount, sum += node -> m_L -> m_Amount * ( id + 1 ) ) );
            else
              node = node -> m_L;
          }
        }


    };        
    CTree                                                 m_Tree;
    unordered_map < Product, size_t >                     m_Products;
    
};

template < typename Product >
void Bestsellers < Product > :: sell ( const Product& p, size_t amount )
{
  auto insert_outcome = m_Products . emplace ( p, amount );
  if ( insert_outcome . second )
    m_Tree . insert ( p, amount );
  else
  {
    if ( amount == 0 )
      return;
    m_Tree . change_amount ( p, ( *insert_outcome . first ) . second, ( *insert_outcome . first ) . second + amount );
    ( *insert_outcome . first ) . second += amount;
  }

}

template < typename Product >
size_t Bestsellers < Product > :: rank ( const Product& p ) const
{
  if ( m_Products . find ( p ) == m_Products . end ( ) )
    throw out_of_range ( "There is no product with that name" );
  size_t amount = m_Products . at ( p );
  return m_Products . size ( ) - m_Tree . find_product ( p, amount );
}

template < typename Product >
const Product& Bestsellers < Product > :: product ( size_t rank ) const
{
  if ( rank > m_Products . size ( ) || rank == 0 )
    throw out_of_range ( "Rank is out of range" );
  return m_Tree . find_rth_product ( rank, m_Products . size ( ) );
}

template < typename Product >
size_t Bestsellers < Product > :: sold ( size_t rank ) const
{
  if ( rank > m_Products . size ( ) || rank == 0 )
    throw out_of_range ( "Rank is out of range" );
  return m_Tree . find_rth_amount ( rank, m_Products . size ( ) ) . m_Amount;
}

template < typename Product >
size_t Bestsellers < Product > :: sold ( size_t from, size_t to ) const
{
  if ( from > m_Products . size ( ) || from == 0 || to > m_Products . size ( ) || to == 0 || from > to )
    throw out_of_range ( "Rank is out of range" );
  size_t sum_from = m_Tree . find_rth_amount ( from, m_Products . size ( ) ) . m_Sum;
  auto sum_to = m_Tree . find_rth_amount ( to, m_Products . size ( ) );
  return sum_from - sum_to . m_Sum + sum_to . m_Amount;
}

template < typename Product >
size_t Bestsellers < Product > :: first_same(size_t r) const 
{
  if ( r > m_Products . size ( ) || r == 0 )
    throw out_of_range ( "Rank is out of range" ); 
  return r - m_Tree . find_rth_amount ( r, m_Products . size ( ) ) . m_ID; 
}

template < typename Product >
size_t Bestsellers < Product > :: last_same(size_t r) const 
{ 
  if ( r > m_Products . size ( ) || r == 0 )
    throw out_of_range ( "Rank is out of range" );
  auto l_s = m_Tree . find_rth_amount ( r, m_Products . size ( ) );
  return r - l_s . m_ID + l_s . m_Size - 1; 
}

template < typename Product >
void Bestsellers < Product > :: CTree :: set_null_on_dad ( shared_ptr < CNode >& node )
{
  node -> m_Dad = nullptr;
  if ( node -> m_L )
    set_null_on_dad ( node -> m_L );
  if ( node -> m_R )
    set_null_on_dad ( node -> m_R );
}

template < typename Product >
void Bestsellers < Product > :: CTree :: change_amount ( const Product& p, size_t prev_amount, size_t new_amount )
{
  remove ( p, prev_amount );
  insert ( p, new_amount );
}

template < typename Product >
void Bestsellers < Product > :: CTree :: insert ( const Product& p, size_t amount, shared_ptr < CNode >& node, shared_ptr < CNode > dad )
{
  if ( ! node )
    node = make_shared < CNode > ( p, amount, dad );
  else if ( node -> m_Amount == amount ) 
    node -> m_Same_Am_Prod . insert ( Product ( p ) );
  else 
  {
    node -> m_Sons_Am += 1;      
    node -> m_Sons_Sum += amount;
    if ( amount < node -> m_Amount )
    {
      insert ( p, amount, node -> m_L, node );
      node -> m_Height = max ( node -> m_R ? node -> m_R -> m_Height + 1 : 0, node -> m_L ? node -> m_L -> m_Height + 1 : 0 );  
      left_balance ( node );
    }
    else
    {
      insert ( p, amount, node -> m_R, node );
      node -> m_Height = max ( node -> m_R ? node -> m_R -> m_Height + 1 : 0, node -> m_L ? node -> m_L -> m_Height + 1 : 0 );  
      right_balance ( node );
    }
  }
}

template < typename Product >
void Bestsellers < Product > :: CTree :: remove ( const Product& p, size_t amount, shared_ptr < CNode >& node, size_t size )
{
  if ( amount != node -> m_Amount )
  {
    node -> m_Sons_Sum -= amount * size;
    node -> m_Sons_Am -= size;
    if ( amount < node -> m_Amount )
    {
      remove ( p, amount, node -> m_L, size );
      node -> m_Height = max ( node -> m_R ? node -> m_R -> m_Height + 1 : 0, node -> m_L ? node -> m_L -> m_Height + 1 : 0 );
      right_balance ( node );
    }
    else if ( amount > node -> m_Amount )
    {
      remove ( p, amount, node -> m_R, size );
      node -> m_Height = max ( node -> m_R ? node -> m_R -> m_Height + 1 : 0, node -> m_L ? node -> m_L -> m_Height + 1 : 0 );
      left_balance ( node );
    }
  }
  else
  {
    if ( node -> m_Same_Am_Prod . size ( ) == 1 )
    {
      auto to_del = node;
      if ( to_del -> m_L && to_del -> m_R )
      {
        to_del = to_del -> m_R;
        while ( to_del -> m_L )
          to_del = to_del -> m_L;
        node -> m_Amount = to_del -> m_Amount;
        swap ( node -> m_Same_Am_Prod, to_del -> m_Same_Am_Prod );
        node -> m_Sons_Am -= node -> m_Same_Am_Prod . size ( );
        node -> m_Sons_Sum -= to_del -> m_Amount * node -> m_Same_Am_Prod . size ( );
        remove ( p, to_del -> m_Amount, node -> m_R, node -> m_Same_Am_Prod . size ( ) );
        node -> m_Height = max ( node -> m_R ? node -> m_R -> m_Height + 1 : 0, node -> m_L ? node -> m_L -> m_Height + 1 : 0 );
        left_balance ( node );
      }
      else if ( node -> m_L )
        node = node -> m_L;
      else
        node = node -> m_R;
      to_del -> m_Dad = to_del -> m_L = to_del -> m_R = nullptr;
    }
    else
      node -> m_Same_Am_Prod . erase ( p );
  }
}

template < typename Product >
void Bestsellers < Product > :: CTree :: left_balance ( shared_ptr < CNode >& node )
{
  if ( ( node -> m_L ? node -> m_L -> m_Height + 1 : 0 ) > 1 + ( node -> m_R ? node -> m_R -> m_Height + 1 : 0 ) )
  {
    if ( ( node -> m_L -> m_R ? node -> m_L -> m_R -> m_Height + 1 : 0 ) - ( node -> m_L -> m_L ? node -> m_L -> m_L -> m_Height + 1 : 0 ) == 1 )
      l_rotation ( node -> m_L );
    r_rotation ( node );
  }
}

template < typename Product >
void Bestsellers < Product > :: CTree :: right_balance ( shared_ptr < CNode >& node )
{
    if ( ( node -> m_R ? node -> m_R -> m_Height + 1 : 0 ) > 1 + ( node -> m_L ? node -> m_L -> m_Height + 1 : 0 ) )
    {
      if ( ( node -> m_R -> m_L ? node -> m_R -> m_L -> m_Height + 1 : 0 ) - ( node -> m_R -> m_R ? node -> m_R -> m_R -> m_Height + 1 : 0 ) == 1 )
        r_rotation ( node -> m_R );
      l_rotation ( node );
    }
}

template < typename Product >
void Bestsellers < Product > :: CTree :: l_rotation ( shared_ptr < CNode >& node )
{
  shared_ptr < CNode > r = node -> m_R;
  node -> m_R = r -> m_L;
  r -> m_L = node;
  if ( node -> m_R )
    node -> m_R -> m_Dad = node;
  r -> m_Dad = node -> m_Dad;
  node -> m_Dad = r;
  node -> m_Height = max ( node -> m_R ? node -> m_R -> m_Height + 1 : 0, node -> m_L ? node -> m_L -> m_Height + 1 : 0 );
  r -> m_Height = max ( r -> m_R ? r -> m_R -> m_Height + 1 : 0, r -> m_L ? r -> m_L -> m_Height + 1 : 0 );
  node -> m_Sons_Am = ( node -> m_R ? node -> m_R -> m_Sons_Am + node -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( node -> m_L ? node -> m_L -> m_Sons_Am + node -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  r -> m_Sons_Am = ( r -> m_R ? r -> m_R -> m_Sons_Am + r -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( r -> m_L ? r -> m_L -> m_Sons_Am + r -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  node -> m_Sons_Sum = ( node -> m_R ? node -> m_R -> m_Sons_Sum + node -> m_R -> m_Amount * node -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( node -> m_L ? node -> m_L -> m_Sons_Sum + node -> m_L -> m_Amount * node -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  r -> m_Sons_Sum = ( r -> m_R ? r -> m_R -> m_Sons_Sum + r -> m_R -> m_Amount * r -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( r -> m_L ? r -> m_L -> m_Sons_Sum + r -> m_L -> m_Amount * r -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  node = r;
}

template < typename Product >
void Bestsellers < Product > :: CTree :: r_rotation ( shared_ptr < CNode >& node )
{
  shared_ptr < CNode > l = node -> m_L;
  node -> m_L = l -> m_R;
  l -> m_R = node;
  if ( node -> m_L )
    node -> m_L -> m_Dad = node;
  l -> m_Dad = node -> m_Dad;
  node -> m_Dad = l;
  node -> m_Height = max ( node -> m_R ? node -> m_R -> m_Height + 1 : 0, node -> m_L ? node -> m_L -> m_Height + 1 : 0 );
  l -> m_Height = max ( l -> m_R ? l -> m_R -> m_Height + 1 : 0, l -> m_L ? l -> m_L -> m_Height + 1 : 0 );
  node -> m_Sons_Am = ( node -> m_R ? node -> m_R -> m_Sons_Am + node -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( node -> m_L ? node -> m_L -> m_Sons_Am + node -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  l -> m_Sons_Am = ( l -> m_R ? l -> m_R -> m_Sons_Am + l -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( l -> m_L ? l -> m_L -> m_Sons_Am + l -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  node -> m_Sons_Sum = ( node -> m_R ? node -> m_R -> m_Sons_Sum + node -> m_R -> m_Amount * node -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( node -> m_L ? node -> m_L -> m_Sons_Sum + node -> m_L -> m_Amount * node -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  l -> m_Sons_Sum = ( l -> m_R ? l -> m_R -> m_Sons_Sum + l -> m_R -> m_Amount * l -> m_R -> m_Same_Am_Prod . size ( ) : 0 ) + ( l -> m_L ? l -> m_L -> m_Sons_Sum + l -> m_L -> m_Amount * l -> m_L -> m_Same_Am_Prod . size ( ) : 0 );
  node = l;
} // size

template < typename Product >
size_t Bestsellers < Product > :: CTree :: find_product ( const Product& p, size_t amount, shared_ptr < CNode > node ) const
{
  if ( amount == node -> m_Amount )
    return ( node -> m_L ? node -> m_L -> m_Sons_Am + node -> m_L -> m_Same_Am_Prod . size ( ) : 0 ) + find_in_set ( node -> m_Same_Am_Prod, p );
  else if ( amount < node -> m_Amount )
    return find_product ( p, amount, node -> m_L );
  else
    return ( node -> m_L ? node -> m_L -> m_Sons_Am + node -> m_L -> m_Same_Am_Prod . size ( ) : 0 ) + node -> m_Same_Am_Prod . size ( ) + find_product ( p, amount, node -> m_R );
}

template < typename Product >
const Product& Bestsellers < Product > :: CTree :: find_rth_product  ( size_t rank, size_t quantity ) const
{
  size_t id = quantity - rank;
  auto node = m_Root;
  while ( true )
  {
    if ( ! node -> m_L || node -> m_L -> m_Same_Am_Prod . size ( ) + node -> m_L -> m_Sons_Am <= id )
    {
      id -= ( node -> m_L ? node -> m_L -> m_Same_Am_Prod . size ( ) + node -> m_L -> m_Sons_Am : 0 );
      if ( node -> m_Same_Am_Prod . size ( ) <= id )
      {
        id -= node -> m_Same_Am_Prod . size ( );
        node = node -> m_R;
      }
      else
        return find_in_set ( node -> m_Same_Am_Prod, id );
    }
    else
      node = node -> m_L;
  }
}

#ifndef __PROGTEST__

void test1() {
  Bestsellers<std::string> T;
  T.sell("coke", 32);
  T.sell("bread", 1);
  assert(T.products() == 2);
  T.sell("ham", 2);
  T.sell("mushrooms", 12);

  assert(T.products() == 4);
  assert(T.rank("ham") == 3);
  assert(T.rank(T.product(3)) == 3);
  assert(T.rank("coke") == 1);
  assert(T.sold(1, 3) == 46);
  assert(T.product(2) == "mushrooms");

  T.sell("ham", 11);
  assert(T.products() == 4);
  assert(T.product(2) == "ham");
  assert(T.sold(2) == 13);
  assert(T.sold(2, 2) == 13);
  assert(T.sold(1, 2) == 45);

}

void test2() {
# define CATCH(expr) \
  try { expr; assert(0); } catch (const std::out_of_range&) { assert(1); };

  Bestsellers<std::string> T;
  T.sell("coke", 32);
  T.sell("bread", 1);

  CATCH(T.rank("ham"));
  CATCH(T.product(3));
  CATCH(T.sold(0));
  CATCH(T.sold(9));
  CATCH(T.sold(0, 1));
  CATCH(T.sold(3, 2));
  CATCH(T.sold(1, 9));

#undef CATCH
}

int main() {
  test1();
  test2();
}

#endif


