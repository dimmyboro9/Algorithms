#ifndef __PROGTEST__

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <variant>
#include <vector>

using State = unsigned int;
using Symbol = uint8_t;

struct NFA {
    std::set<State> m_States;
    std::set<Symbol> m_Alphabet;
    std::map<std::pair<State, Symbol>, std::set<State>> m_Transitions;
    State m_InitialState;
    std::set<State> m_FinalStates;
};

struct DFA {
    std::set<State> m_States;
    std::set<Symbol> m_Alphabet;
    std::map<std::pair<State, Symbol>, State> m_Transitions;
    State m_InitialState;
    std::set<State> m_FinalStates;
};

#endif

using namespace std;

NFA to_full_defined ( const NFA& a, const set < Symbol >& alphabet )
{
    NFA full_defined;
    State empty_state = *a . m_States . rbegin ( ) + 1;
    full_defined . m_States = a . m_States;
    full_defined . m_InitialState = a . m_InitialState;
    full_defined . m_FinalStates = a . m_FinalStates;
    full_defined . m_Transitions = a . m_Transitions;
    full_defined . m_Alphabet = alphabet;
    full_defined . m_States . insert ( empty_state );
    for ( auto state : full_defined . m_States )
        for ( auto sym : full_defined . m_Alphabet )
            if ( full_defined . m_Transitions . find ( make_pair ( state, sym ) ) == full_defined . m_Transitions . end ( ) )
                full_defined . m_Transitions . emplace ( make_pair ( state, sym ), set < State > { empty_state } );
    return full_defined;
}

bool is_dfa ( const NFA& a )
{
    for ( auto transition : a . m_Transitions )
        if ( transition . second . size ( ) > 1 )
            return false;
    return true;
}

set < Symbol > set_unify ( const set < Symbol >& a, const set < Symbol >& b )
{
    set < Symbol > result;
    for ( auto it_a = a . begin ( ), it_b = b . begin ( ); it_a != a . end ( ) || it_b != b . end ( );  )
        if ( ( it_b == b . end ( ) ) || ( it_a != a . end ( ) && *it_b > *it_a ) )
        {
            result . insert ( *it_a );
            it_a++;
        }
        else if ( ( it_a == a . end ( ) ) || ( it_b != b . end ( ) && *it_a > *it_b ) )
        {
            result . insert ( *it_b );
            it_b++;
        }
        else
        {
            result . insert ( *it_a );
            it_a++;
            it_b++;
        }
    return result;
}

set < Symbol > set_intersect ( const set < Symbol >& a, const set < Symbol >& b )
{
    set < Symbol > result;
    for ( auto it_a = a . begin ( ), it_b = b . begin ( ); it_a != a . end ( ) && it_b != b . end ( );  )
        if ( *it_a == *it_b )
        {
            result . insert ( *it_a );
            it_a++;
            it_b++;
        }
        else if ( *it_a > *it_b )
            it_b++;
        else
            it_a++;
    return result;
}

NFA convert_to_long_int ( const NFA& a, const NFA& b, const map < pair < State, State >, State >& convert_table, const map < pair < pair < State, State >, Symbol >, set < pair < State, State > > >& transitions, const set < pair < State, State > >& final_states )
{
    NFA result { {}, set_unify ( a . m_Alphabet, b . m_Alphabet ), {}, 0, {} };
    for ( auto state : convert_table )
        result . m_States . insert ( state . second );
    for ( auto transition : transitions )
        for ( auto one_transition : transition . second )
            result . m_Transitions [ make_pair ( convert_table . at ( transition . first . first ), transition . first . second ) ] . emplace ( convert_table . at ( one_transition ) );
    for ( auto final_state : final_states )
        result . m_FinalStates . insert ( convert_table . at ( final_state ) );
    return result;
}

NFA parallel_run ( const NFA& a, const NFA& b, bool is_unify, set < Symbol > alphabet )
{
    State convert_num = 0;
    map < pair < State, State >, State > convert_table;
    map < pair < pair < State, State >, Symbol >, set < pair < State, State > > > transitions;
    queue < pair < State, State > > to_intersect;
    set < pair < State, State > > final_states;
    convert_table . emplace ( make_pair ( a . m_InitialState, b . m_InitialState ), convert_num++ );
    to_intersect . push ( make_pair ( a . m_InitialState, b . m_InitialState ) );
    if ( is_unify )
    {
        if ( a . m_FinalStates . find ( a . m_InitialState ) != a . m_FinalStates . end ( ) || b . m_FinalStates . find ( b . m_InitialState ) != b . m_FinalStates . end ( ) )
            final_states . emplace ( a . m_InitialState, b . m_InitialState );
    }
    else
        if ( a . m_FinalStates . find ( a . m_InitialState ) != a . m_FinalStates . end ( ) && b . m_FinalStates . find ( b . m_InitialState ) != b . m_FinalStates . end ( ) )
            final_states . emplace ( a . m_InitialState, b . m_InitialState );
    while ( to_intersect . size ( ) )
    {
        pair < State, State > state = to_intersect . front ( );
        for ( auto sym : alphabet )
        {
            auto has_transition_a = a . m_Transitions . find ( make_pair ( state . first, sym ) );
            auto has_transition_b = b . m_Transitions . find ( make_pair ( state . second, sym ) );
            if ( has_transition_a != a . m_Transitions . end ( ) && has_transition_b != b . m_Transitions . end ( ) )
            {
                set < State > a_transition = ( *has_transition_a ) . second;
                set < State > b_transition = ( *has_transition_b ) . second;
                for ( auto f : a_transition )
                    for ( auto s : b_transition )
                    {
                        // possible find 
                        auto is_emplaced = convert_table . emplace ( make_pair ( f, s ), convert_num );
                        if ( is_emplaced . second )
                        {
                            if ( is_unify )
                            {
                                if ( a . m_FinalStates . find ( f ) != a . m_FinalStates . end ( ) || b . m_FinalStates . find ( s ) != b . m_FinalStates . end ( ) )
                                    final_states . emplace ( f, s );
                            }
                            else
                                if ( a . m_FinalStates . find ( f ) != a . m_FinalStates . end ( ) && b . m_FinalStates . find ( s ) != b . m_FinalStates . end ( ) )
                                    final_states . emplace ( f, s );
                            to_intersect . push ( make_pair ( f, s ) );
                            convert_num++;
                        }
                        transitions [ make_pair ( state, sym ) ] . emplace ( f, s );
                    }
            }
        }
        to_intersect . pop ( );
    }
    return convert_to_long_int ( a, b, convert_table, transitions, final_states );
}

void dfa_2_dfa ( const NFA& nfa, DFA& dfa )
{
    dfa . m_States = nfa . m_States;
    dfa . m_FinalStates = nfa . m_FinalStates;
    for ( auto transition : nfa . m_Transitions )
        dfa . m_Transitions . emplace ( transition . first, *transition . second . begin ( ) );
}

void determinize ( const NFA& nfa, map < set < State >, State >& convert_table, map < pair < set < State >, Symbol >, set < State > >& transitions, queue < set < State > > to_determinize, State convert_num )
{
    while ( to_determinize . size ( ) )
    {
        set < State > state = to_determinize . front ( );
        for ( auto sym : nfa . m_Alphabet )
        {
            set < State > new_state;
            for ( auto st : state )
                if ( nfa . m_Transitions . find ( make_pair ( st, sym ) ) != nfa . m_Transitions . end ( ) )
                    for ( auto s : nfa . m_Transitions . at ( make_pair ( st, sym ) ) )
                        new_state . insert ( s ); 
            if ( new_state . size ( ) )
            {
                auto is_emplaced = convert_table . emplace ( new_state, convert_num );
                if ( is_emplaced . second )
                {
                    to_determinize . push ( new_state );
                    convert_num++;
                }
                transitions . emplace ( make_pair ( state, sym ), new_state );
            }
        }
        to_determinize . pop ( );
    }
}

void convert_set_2_state ( DFA& dfa, const NFA& nfa, const map < set < State >, State >& convert_table, const map < pair < set < State >, Symbol >, set < State > >& transitions )
{
    for ( auto state : convert_table )
    {
        dfa . m_States . insert ( state . second );
        for ( auto st = state . first . begin ( ), fst = nfa . m_FinalStates . begin ( ); st != state . first . end ( ) && fst != nfa . m_FinalStates . end ( ); )
            if ( *st == *fst )
            {
                dfa . m_FinalStates . insert ( state . second );
                break;
            }
            else if ( *st > *fst )
                fst++;
            else
                st++;
    }
    for ( auto transition : transitions )
        dfa . m_Transitions . emplace ( make_pair ( convert_table . at ( transition . first . first ), transition . first . second ), convert_table . at ( transition . second ) );
}

void nfa_2_dfa ( const NFA& nfa, DFA& dfa )
{
    State convert_num = 0;
    map < set < State >, State > convert_table;
    map < pair < set < State >, Symbol >, set < State > > transitions;
    queue < set < State > > to_determinize;
    convert_table . emplace ( set < State > { nfa . m_InitialState }, convert_num++ );
    to_determinize . push ( { nfa . m_InitialState } );
    determinize ( nfa, convert_table, transitions, to_determinize, convert_num );
    convert_set_2_state ( dfa, nfa, convert_table, transitions );
}

bool path_to_final ( State state, const set < Symbol >& alphabet, const map < pair < State, Symbol >, State >& transitions, const set < State >& final_states, set < State >& is_visited )
{
    if ( final_states . find ( state ) != final_states . end ( ) )
        return true;
    is_visited . insert ( state );
    for ( auto sym : alphabet )
    {
        auto it_to_next = transitions . find ( make_pair ( state, sym ) );
        if ( it_to_next != transitions . end ( ) && is_visited . find ( ( *it_to_next ) . second ) == is_visited . end ( ) )
            if ( path_to_final ( ( *it_to_next ) . second, alphabet, transitions, final_states, is_visited ) )
                return true;
    }
    return false;
}

void delete_useless_states ( DFA& dfa )
{
    set < State > useless;
    for ( auto state = dfa . m_States . begin ( ); state != dfa . m_States . end ( );  )
    {
        set < State > is_visited;
        if ( useless . find ( *state ) != useless . end ( ) || ! path_to_final ( *state, dfa . m_Alphabet, dfa . m_Transitions, dfa . m_FinalStates, is_visited ) )
        {
            for ( auto iv : is_visited )
                useless . insert ( iv );
            auto to_del = state;
            state++;
            dfa . m_States . erase ( to_del );
        }
        else
            state++;
    }
    for ( auto transition = dfa . m_Transitions . begin ( ); transition != dfa . m_Transitions . end ( ); )
    {
        if ( useless . find ( ( *transition ) . first . first ) != useless . end ( ) || useless . find ( ( *transition ) . second ) != useless . end ( ) )
        {
            auto to_del = transition;
            transition++;
            dfa . m_Transitions . erase ( to_del );
        }
        else
            transition++;
    }
}

void initialize_new_states ( const DFA& dfa, State& final, State& not_final, map < State, pair < State, bool > >& new_states, const State MAX_STATE )
{
    for ( auto st : dfa . m_States )
    {
        if ( dfa . m_FinalStates . find ( st ) != dfa . m_FinalStates . end ( ) )
            if ( final == MAX_STATE )
            {
                new_states . emplace ( st, make_pair ( st, true ) );
                final = st;
            }
            else
                new_states . emplace ( st, make_pair ( final, true ) );
        else
            if ( not_final == MAX_STATE )
            {
                new_states . emplace ( st, make_pair ( st, false ) );
                not_final = st;
            }
            else
                new_states . emplace ( st, make_pair ( not_final, false ) );
    }
}

void initialize_new_transition ( const DFA& dfa, map < State, vector < State > >& transitions, const map < State, pair < State, bool > >& new_states, State state, const State MAX_STATE )
{
    size_t pos = 0;
    if ( transitions . find ( state ) == transitions . end ( ) )
        transitions . emplace ( state, vector < State > ( dfa . m_Alphabet . size ( ) ) );
    for ( auto sym : dfa . m_Alphabet )
    {
        if ( dfa . m_Transitions . find ( make_pair ( state, sym ) ) != dfa . m_Transitions . end ( ) )
            transitions [ state ] [ pos ] = new_states . at ( dfa . m_Transitions . at ( make_pair ( state, sym ) ) ) . first;
        else 
            transitions [ state ] [ pos ] = MAX_STATE;
        pos++;
    }
}

void find_equivalent_states ( const DFA& dfa, map < State, pair < State, bool > >& new_states, map < State, vector < State > >& new_transitions, const State MAX_STATE )
{
    map < State, State > tmp_states;
    map < State, vector < State > > tmp_transitions;
    for ( auto st : dfa . m_States )
    {
        initialize_new_transition ( dfa, tmp_transitions, new_states, st, MAX_STATE );
        if ( tmp_transitions . at ( st ) == new_transitions . at ( new_states . at ( st ) . first ) )
            tmp_states [ st ] = new_states . at ( st ) . first;
        else
        {
            bool need_to_add = true;
            for ( auto n_tr : new_transitions )
                if ( n_tr . first != MAX_STATE )
                    if ( new_states . at ( st ) . second == new_states . at ( n_tr . first ) . second && tmp_transitions . at ( st ) == n_tr . second )
                    {
                        tmp_states [ st ] = n_tr . first;
                        need_to_add = false;
                        break;
                    }
            if ( need_to_add )
            {
                tmp_states [ st ] = st;
                new_transitions . emplace ( st, tmp_transitions . at ( st ) );
            }
        }
    }
    auto t_st = tmp_states . begin ( );
    for ( auto n_st = new_states . begin ( ); n_st != new_states . end ( ) && t_st != tmp_states . end ( ); n_st++, t_st++ )
        ( *n_st ) . second . first = ( *t_st ) . second;
    for ( auto tr : new_transitions )
        initialize_new_transition ( dfa, new_transitions, new_states, tr . first, MAX_STATE );
}

set < State > change_states ( const map < State, vector < State > >& new_transitions, const State MAX_STATE )
{
    set < State > to_return;
    for ( auto new_transition : new_transitions )
        if ( new_transition . first != MAX_STATE )
            to_return . insert ( new_transition . first );
    return to_return;
}

map < pair < State, Symbol >, State > change_transitions ( const set < Symbol >& alphabet, const map < State, pair < State, bool > >& new_states, const map < State, vector < State > >& new_transitions, const State MAX_STATE )
{
    map < pair < State, Symbol >, State > to_return;
    for ( auto new_transition : new_transitions )
    {
        size_t pos = 0;
        for ( auto sym : alphabet )
        {
            if ( new_transition . second . at ( pos ) != MAX_STATE )
                to_return . emplace ( make_pair ( new_transition . first, sym ), new_transition . second . at ( pos ) );
            pos++;
        }
    }
    return to_return;
}

set<State> change_final_states ( const map < State, pair < State, bool > >& new_states, const map < State, vector < State > >& new_transitions, const State MAX_STATE )
{
    set < State > to_return;
    for ( auto new_transition : new_transitions )
        if ( new_transition . first != MAX_STATE )
            if ( new_states . at ( new_transition . first ) . second )
                to_return . insert ( new_transition . first );
    return to_return;
}

void minimilize ( DFA& dfa )
{
    const State MAX_STATE = *dfa . m_States . rbegin ( ) + 1;
    map < State, pair < State, bool > > new_states;
    map < State, vector < State > > new_transitions;
    size_t prev_size = new_transitions . size ( );
    State final = MAX_STATE, not_final = MAX_STATE;
    initialize_new_states ( dfa, final, not_final, new_states, MAX_STATE );
    initialize_new_transition ( dfa, new_transitions, new_states, final, MAX_STATE );
    initialize_new_transition ( dfa, new_transitions, new_states, not_final, MAX_STATE );
    size_t size = new_transitions . size ( );
    while ( size > prev_size )
    {
        prev_size = size;
        find_equivalent_states ( dfa, new_states, new_transitions, MAX_STATE );
        size = new_transitions . size ( );
    }
    dfa . m_States = change_states ( new_transitions, MAX_STATE );
    dfa . m_Transitions = change_transitions ( dfa . m_Alphabet, new_states, new_transitions, MAX_STATE );
    dfa . m_FinalStates = change_final_states ( new_states, new_transitions, MAX_STATE );
}

DFA unify ( const NFA& a, const NFA& b )
{
    set < Symbol > alphabet = set_unify ( a . m_Alphabet, b . m_Alphabet );
    if ( ! ( a . m_FinalStates . size ( ) || b . m_FinalStates . size ( ) /* && a . m_Transitions . size ( ) && b . m_Transitions . size ( ) */ ) )
        return DFA { { 0 }, alphabet, { }, 0, { } };
    NFA full_a = to_full_defined ( a, alphabet );
    NFA full_b = to_full_defined ( b, alphabet );
    DFA dfa_unify;
    bool is_unify_dfa = is_dfa ( full_a ) && is_dfa ( full_b );
    NFA nfa_unify = parallel_run ( full_a, full_b, true, full_a . m_Alphabet );
    if ( ! ( nfa_unify . m_FinalStates . size ( ) /* && nfa_unify . m_Transitions . size ( ) */ ) )
        return DFA { { 0 }, alphabet, { }, 0, { } };
    dfa_unify . m_Alphabet = nfa_unify . m_Alphabet;
    dfa_unify . m_InitialState = nfa_unify . m_InitialState;
    if ( is_unify_dfa )
        dfa_2_dfa ( nfa_unify, dfa_unify );
    else
        nfa_2_dfa ( nfa_unify, dfa_unify );
    delete_useless_states ( dfa_unify );
    if ( ! dfa_unify . m_Transitions . size ( ) && dfa_unify . m_FinalStates . find ( dfa_unify . m_InitialState ) != dfa_unify . m_FinalStates . end ( ) )
        return DFA { { 0 }, alphabet, { }, 0, { 0 } };
    minimilize ( dfa_unify );
    return dfa_unify;
}

DFA intersect ( const NFA& a, const NFA& b )
{
    if ( ! ( a . m_FinalStates . size ( ) && b . m_FinalStates . size ( ) /* && a . m_Transitions . size ( ) && b . m_Transitions . size ( ) */ ) )
        return DFA { { 0 }, set_unify ( a . m_Alphabet, b . m_Alphabet ), { }, 0, { } };
    DFA dfa_intersect;
    bool is_intersect_dfa = is_dfa ( a ) && is_dfa ( b );
    NFA nfa_intersect = parallel_run ( a, b, false, set_intersect ( a . m_Alphabet, b . m_Alphabet ) );
    if ( ! nfa_intersect . m_Transitions . size ( ) && nfa_intersect . m_FinalStates . find ( nfa_intersect . m_InitialState ) != nfa_intersect . m_FinalStates . end ( ) )
        return DFA { { 0 }, set_unify ( a . m_Alphabet, b . m_Alphabet ), { }, 0, { 0 } };
    if ( ! ( nfa_intersect . m_FinalStates . size ( ) /* &&  nfa_intersect . m_Transitions . size ( ) */ ) )
        return DFA { { 0 }, set_unify ( a . m_Alphabet, b . m_Alphabet ), { }, 0, { } };
    dfa_intersect . m_Alphabet = nfa_intersect . m_Alphabet;
    dfa_intersect . m_InitialState = nfa_intersect . m_InitialState;
    if ( is_intersect_dfa )
        dfa_2_dfa ( nfa_intersect, dfa_intersect );
    else
        nfa_2_dfa ( nfa_intersect, dfa_intersect );
    delete_useless_states ( dfa_intersect );
    minimilize ( dfa_intersect );
    return dfa_intersect;
}

#ifndef __PROGTEST__

// You may need to update this function or the sample data if your state naming strategy differs.
bool operator==(const DFA& a, const DFA& b)
{
    return std::tie(a.m_States, a.m_Alphabet, a.m_Transitions, a.m_InitialState, a.m_FinalStates) == std::tie(b.m_States, b.m_Alphabet, b.m_Transitions, b.m_InitialState, b.m_FinalStates);
}

int main()
{
    // DFA a10{
    //     {0, 1, 2, 3, 4, 5, 6, 7},
    //     {'a', 'b'},
    //     {
    //         {{0, 'a'}, {1}},
    //         {{1, 'a'}, {2}},
    //         {{2, 'a'}, {0}},
    //         {{2, 'b'}, {3}},
    //         {{3, 'a'}, {4}},
    //         {{3, 'b'}, {5}},
    //         {{4, 'a'}, {6}},
    //         {{4, 'b'}, {7}},
    //         {{5, 'a'}, {7}},
    //         {{6, 'b'}, {6}},
    //         {{7, 'a'}, {5}},
    //         {{7, 'b'}, {7}},
    //     },
    //     0,
    //     {2},
    // };
    // delete_useless_states ( a10 );
    NFA a1{
        {0, 1, 2},
        {'a', 'b'},
        {
            {{0, 'a'}, {0, 1}},
            {{0, 'b'}, {0}},
            {{1, 'a'}, {2}},
        },
        0,
        {2},
    };
    NFA a2{
        {0, 1, 2},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{1, 'a'}, {2}},
            {{2, 'a'}, {2}},
            {{2, 'b'}, {2}},
        },
        0,
        {2},
    };
    DFA a{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{1, 'a'}, {2}},
            {{2, 'a'}, {2}},
            {{2, 'b'}, {3}},
            {{3, 'a'}, {4}},
            {{3, 'b'}, {3}},
            {{4, 'a'}, {2}},
            {{4, 'b'}, {3}},
        },
        0,
        {2},
    };
    assert(intersect(a1, a2) == a);

    NFA b1{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{0, 'b'}, {2}},
            {{2, 'a'}, {2, 3}},
            {{2, 'b'}, {2}},
            {{3, 'a'}, {4}},
        },
        0,
        {1, 4},
    };
    NFA b2{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'b'}, {1}},
            {{1, 'a'}, {2}},
            {{2, 'b'}, {3}},
            {{3, 'a'}, {4}},
            {{4, 'a'}, {4}},
            {{4, 'b'}, {4}},
        },
        0,
        {4},
    };
    DFA b{
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{0, 'b'}, {2}},
            {{2, 'a'}, {3}},
            {{2, 'b'}, {4}},
            {{3, 'a'}, {5}},
            {{3, 'b'}, {6}},
            {{4, 'a'}, {7}},
            {{4, 'b'}, {4}},
            {{5, 'a'}, {5}},
            {{5, 'b'}, {4}},
            {{6, 'a'}, {8}},
            {{6, 'b'}, {4}},
            {{7, 'a'}, {5}},
            {{7, 'b'}, {4}},
            {{8, 'a'}, {8}},
            {{8, 'b'}, {8}},
        },
        0,
        {1, 5, 8},
    };
    assert(unify(b1, b2) == b);

    NFA c1{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{0, 'b'}, {2}},
            {{2, 'a'}, {2, 3}},
            {{2, 'b'}, {2}},
            {{3, 'a'}, {4}},
        },
        0,
        {1, 4},
    };
    NFA c2{
        {0, 1, 2},
        {'a', 'b'},
        {
            {{0, 'a'}, {0}},
            {{0, 'b'}, {0, 1}},
            {{1, 'b'}, {2}},
        },
        0,
        {2},
    };
    DFA c{
        {0},
        {'a', 'b'},
        {},
        0,
        {},
    };
    assert(intersect(c1, c2) == c);

    NFA d1{
        {0, 1, 2, 3},
        {'i', 'k', 'q'},
        {
            {{0, 'i'}, {2}},
            {{0, 'k'}, {1, 2, 3}},
            {{0, 'q'}, {0, 3}},
            {{1, 'i'}, {1}},
            {{1, 'k'}, {0}},
            {{1, 'q'}, {1, 2, 3}},
            {{2, 'i'}, {0, 2}},
            {{3, 'i'}, {3}},
            {{3, 'k'}, {1, 2}},
        },
        0,
        {2, 3},
    };
    NFA d2{
        {0, 1, 2, 3},
        {'i', 'k'},
        {
            {{0, 'i'}, {3}},
            {{0, 'k'}, {1, 2, 3}},
            {{1, 'k'}, {2}},
            {{2, 'i'}, {0, 1, 3}},
            {{2, 'k'}, {0, 1}},
        },
        0,
        {2, 3},
    };
    DFA d{
        {0, 1, 2, 3},
        {'i', 'k', 'q'},
        {
            {{0, 'i'}, {1}},
            {{0, 'k'}, {2}},
            {{2, 'i'}, {3}},
            {{2, 'k'}, {2}},
            {{3, 'i'}, {1}},
            {{3, 'k'}, {2}},
        },
        0,
        {1, 2, 3},
    };
    assert(intersect(d1, d2) == d);
}
#endif
