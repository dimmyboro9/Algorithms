// This program needs to have checks for incorrect inputs added, however, I don't have time to do this now, so we'll leave it for the future (or maybe forever :((( )

#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <random>

using namespace std;

const int AMOUNT_OF_FILES = 13;
const vector <pair <int, int>> NEIGHBOURS = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
const string DIRECTORY = "maps/";
const string TXT = ".txt";

bool visual_menu ( bool& till_the_end );
bool read_write_file ( const string& file_name, ostream& os );

struct TBox
{
                            TBox ( char icon, bool access, size_t x, size_t y )
                            : m_Icon ( icon )
                            , m_Access ( access )
                            , m_Value ( 0 )
                            , m_Pos ( x, y ) { };
    char                    m_Icon;
    bool                    m_Access;
    size_t                  m_Value;
    pair<size_t, size_t>    m_Pos;
    pair<size_t, size_t>    m_Prev;
};

class CMap
{
    public:
                                                CMap ( ) = default;
        bool                                    init_map ( const string& file_name );
        bool                                    run_algorithm ( size_t& algorithm_id );
        void                                    print_map ( ostream& os ) const;
        [[nodiscard]] size_t                    manhattan_endpoint_distance ( const pair <size_t, size_t>& pos ) const;
    private:
        bool                                    define_problem ( const string& line );
        bool                                    random_search ( );
        static shared_ptr <TBox>                random_erase_from_vector ( vector <shared_ptr <TBox>>& array );
        bool                                    display_expanded_node ( bool& till_the_end, const shared_ptr <TBox>& box );
        static void                             change_point ( const shared_ptr <TBox>& neighbour_box, const shared_ptr <TBox>& box );
        static void                             change_endpoint ( const shared_ptr <TBox>& neighbour_box, const shared_ptr <TBox>& box, bool& path_found );
        bool                                    bfs ( );
        bool                                    dfs ( );
        bool                                    dfs_visit ( const shared_ptr <TBox>& box, bool& till_the_end );
        bool                                    greedy_search ( );
        bool                                    a_star ( );
        void                                    reconstruct_path ( );
        bool                                    print_results ( ostream& os ) const;

        vector <vector <shared_ptr <TBox> > >   m_Map;
        pair <size_t, size_t>                   m_Start;
        pair <size_t, size_t>                   m_End;
        size_t                                  m_Box_counter = 0;
};

class compare_in_greedy
{
    public:
        explicit compare_in_greedy ( CMap& obj ) : m_Obj ( obj ) { }
        bool    operator ( ) ( const shared_ptr<TBox>& a, const shared_ptr<TBox>& b ) const
        {
            return m_Obj . manhattan_endpoint_distance ( a -> m_Pos ) > m_Obj . manhattan_endpoint_distance ( b -> m_Pos );
        }

    private:
        const CMap& m_Obj;
};

class compare_in_astar
{
    public:
        explicit compare_in_astar ( CMap& obj ) : m_Obj ( obj ) { }
        bool    operator ( ) ( const shared_ptr<TBox>& a, const shared_ptr<TBox>& b ) const
        {
            return m_Obj . manhattan_endpoint_distance ( a -> m_Pos ) + a -> m_Value > m_Obj . manhattan_endpoint_distance ( b -> m_Pos ) + b -> m_Value;
        }
    private:
        const CMap& m_Obj;
};

bool CMap:: init_map ( const string& file_name )
{
    ifstream file ( file_name );
    if ( ! file . good ( ) )
        return false;
    string line;
    for ( size_t x = 0; getline ( file, line ); x++ )
    {
        if ( ( line . substr ( 0, strlen ( "start" ) ) == "start" || line . substr ( 0, strlen ( "end" ) ) == "end" ) )
        {
            if ( ! define_problem ( line ) )
            {
                file . close ( );
                return false;
            }
        }
        else
        {
            m_Map . emplace_back ( );
            for ( size_t y = 0; y < line . size ( ); y++ )
                m_Map . at ( x ) . emplace_back ( new TBox ( line[y], line[y] != 'X', x, y ) );
        }
    }
    m_Map . at ( m_Start . first ) . at ( m_Start . second ) -> m_Access = false;
    file . close ( );
    return true;
}

bool CMap:: define_problem ( const string& line )
{
    string condition;
    char comma;
    size_t x, y;
    istringstream iss ( line );
    iss >> condition >> y >> comma >> x;
    if ( x >= m_Map . size ( ) || y >= m_Map . at ( x ) . size ( ) || ! m_Map . at ( x ) . at ( y ) -> m_Access )
        return false;
    if ( condition == "start" )
    {
        m_Start = make_pair ( x, y );
        m_Map . at ( x ) . at ( y ) -> m_Icon = 'S';
        m_Map . at ( x ) . at ( y ) -> m_Value = 0;
        return true;
    }
    m_End = make_pair ( x, y );
    m_Map . at ( x ) . at ( y ) -> m_Icon = 'E';
    return true;
}

bool CMap:: run_algorithm ( size_t& algorithm_id )
{
    if ( m_Start != m_End )
    {
        switch ( algorithm_id )
        {
            case 1:
                if ( ! random_search ( ) )
                    return false;
                break;
            case 2:
                if ( ! bfs ( ) )
                    return false;
                break;
            case 3:
                if ( ! dfs ( ) )
                    return false;
                break;
            case 4:
                if ( ! greedy_search ( ) )
                    return false;
                break;
            case 5:
                if ( ! a_star ( ) )
                    return false;
                break;
            default:
                cout << "This algorithm doesn't exist" << endl;
                return false;
        }
        reconstruct_path ( );
    }
    if ( ! print_results ( cout ) )
        return false;
    return true;
}

bool CMap::random_search ( )
{
    vector <shared_ptr <TBox> > to_visit;
    to_visit . emplace_back ( m_Map . at ( m_Start . first ) . at ( m_Start . second ) );
    bool till_the_end = false, path_found = false;

    while ( ! to_visit . empty ( ) )
    {
        auto box = random_erase_from_vector ( to_visit );
        if ( ! display_expanded_node ( till_the_end, box ) )
            return false;
        for ( auto neighbour : NEIGHBOURS )
        {
            shared_ptr <TBox> neighbour_box = m_Map . at ( box -> m_Pos . first + neighbour . first ) . at ( box -> m_Pos . second + neighbour . second );
            if ( neighbour_box -> m_Pos == m_End )
            {
                change_endpoint ( neighbour_box, box, path_found );
                break;
            }
            if ( neighbour_box -> m_Access )
            {
                change_point ( neighbour_box, box );
                to_visit . emplace_back ( neighbour_box );
            }
        }
        if ( path_found )
            break;
    }
    if ( ! path_found )
    {
        cout << "There is no path between start and end points" << endl;
        return false;
    }
    return true;
}

shared_ptr <TBox> CMap:: random_erase_from_vector ( vector <shared_ptr <TBox>>& array )
{
    random_device rd;
    mt19937 gen( rd ( ) );
    uniform_int_distribution <size_t> dist ( 0, array.size() - 1 );
    size_t random_number = dist ( gen );

    auto to_return = array . at ( random_number );
    swap( array . at ( random_number ), array . back ( ) );
    array . pop_back ( );
    return to_return;
}

bool CMap:: display_expanded_node ( bool& till_the_end, const shared_ptr <TBox>& box )
{
    if ( ! till_the_end )
        if ( ! visual_menu ( till_the_end ) )
            return false;
    m_Box_counter++;
    if ( box -> m_Icon != 'S')
        box -> m_Icon = '#';
    print_map ( cout );
    return true;
}

void CMap:: change_point ( const shared_ptr <TBox>& neighbour_box, const shared_ptr <TBox>& box )
{
    neighbour_box -> m_Prev = box -> m_Pos;
    neighbour_box -> m_Access = false;
    neighbour_box -> m_Value = box -> m_Value + 1;
}

void CMap:: change_endpoint ( const shared_ptr <TBox>& neighbour_box, const shared_ptr <TBox>& box, bool& path_found )
{
    neighbour_box -> m_Prev = box -> m_Pos;
    neighbour_box -> m_Value = box -> m_Value + 1;
    path_found = true;
}

bool CMap:: bfs ( )
{
    queue <shared_ptr <TBox> > to_visit;
    to_visit . emplace ( m_Map . at ( m_Start . first ) . at ( m_Start . second ) );
    bool till_the_end = false, path_found = false;

    while ( ! to_visit . empty ( ) )
    {
        auto box = to_visit . front ( );
        to_visit . pop ( );
        if ( ! display_expanded_node ( till_the_end, box ) )
            return false;
        for ( auto neighbour : NEIGHBOURS )
        {
            shared_ptr <TBox> neighbour_box = m_Map . at ( box -> m_Pos . first + neighbour . first ) . at ( box -> m_Pos . second + neighbour . second );
            if ( neighbour_box -> m_Pos == m_End )
            {
                change_endpoint ( neighbour_box, box, path_found );
                break;
            }
            if ( neighbour_box -> m_Access )
            {
                change_point ( neighbour_box, box );
                to_visit . emplace ( neighbour_box );
            }
        }
        if ( path_found )
            break;
    }
    if ( ! path_found )
    {
        cout << "There is no path between start and end points" << endl;
        return false;
    }
    return true;
}

bool CMap:: dfs ( )
{
    bool till_the_end = false;
    if ( ! dfs_visit ( m_Map . at ( m_Start . first ) . at ( m_Start . second ), till_the_end ) )
        return false;
    return true;
}

bool CMap:: dfs_visit ( const shared_ptr <TBox>& box, bool& till_the_end )
{
    print_map ( cout );
    if ( ! till_the_end )
        if ( ! visual_menu ( till_the_end ) )
            return false;
    for ( auto neighbour : NEIGHBOURS )
    {
        shared_ptr <TBox> neighbour_box = m_Map . at ( box -> m_Pos . first + neighbour . first ) . at ( box -> m_Pos . second + neighbour . second );
        if ( neighbour_box -> m_Pos == m_End )
        {
            neighbour_box -> m_Prev = box -> m_Pos;
            neighbour_box -> m_Value = box -> m_Value + 1;
            m_Box_counter++;
            return true;
        }
        if ( neighbour_box -> m_Access )
        {
            neighbour_box -> m_Icon = '#';
            neighbour_box -> m_Prev = box -> m_Pos;
            neighbour_box -> m_Value = box -> m_Value + 1;
            neighbour_box -> m_Access = false;
            m_Box_counter++;
            if ( dfs_visit ( neighbour_box, till_the_end ) )
                return true;
        }
    }
    return false;
}

bool CMap:: greedy_search ( )
{
    priority_queue <shared_ptr <TBox>, vector <shared_ptr <TBox>>, compare_in_greedy> to_visit { compare_in_greedy ( *this ) };
    to_visit . emplace ( m_Map . at ( m_Start . first ) . at ( m_Start . second ) );
    bool till_the_end = false, path_found = false;

    while ( ! to_visit . empty ( ) )
    {
        auto box = to_visit . top ( );
        to_visit . pop ( );
        if ( ! display_expanded_node ( till_the_end, box ) )
            return false;
        for ( auto neighbour : NEIGHBOURS )
        {
            shared_ptr <TBox> neighbour_box = m_Map . at ( box -> m_Pos . first + neighbour . first ) . at ( box -> m_Pos . second + neighbour . second );
            if ( neighbour_box -> m_Pos == m_End )
            {
                change_endpoint ( neighbour_box, box, path_found );
                break;
            }
            if ( neighbour_box -> m_Access )
            {
                change_point ( neighbour_box, box );
                to_visit . emplace ( neighbour_box );
            }
        }
        if ( path_found )
            break;
    }
    if ( ! path_found )
    {
        cout << "There is no path between start and end points" << endl;
        return false;
    }
    return true;
}

bool CMap:: a_star ( )
{
    priority_queue <shared_ptr <TBox>, vector <shared_ptr <TBox>>, compare_in_astar> to_visit { compare_in_astar ( *this ) };
    to_visit . emplace ( m_Map . at ( m_Start . first ) . at ( m_Start . second ) );
    bool till_the_end = false, path_found = false;

    while ( ! to_visit . empty ( ) )
    {
        auto box = to_visit . top ( );
        to_visit . pop ( );
        if ( ! box -> m_Access && box -> m_Pos != m_Start )
            continue;
        if ( ! display_expanded_node ( till_the_end, box ) )
            return false;
        box -> m_Access = false;
        for ( auto neighbour : NEIGHBOURS )
        {
            shared_ptr <TBox> neighbour_box = m_Map . at ( box -> m_Pos . first + neighbour . first ) . at ( box -> m_Pos . second + neighbour . second );
            if ( neighbour_box -> m_Pos == m_End )
            {
                change_endpoint ( neighbour_box, box, path_found );
                break;
            }
            if ( neighbour_box -> m_Access )
            {
                if ( ! neighbour_box -> m_Value || box -> m_Value + 1 < neighbour_box -> m_Value )
                {
                    neighbour_box -> m_Prev = box -> m_Pos;
                    neighbour_box -> m_Value = box -> m_Value + 1;
                    to_visit . emplace ( neighbour_box );
                }
            }
        }
        if ( path_found )
            break;
    }
    if ( ! path_found )
    {
        cout << "There is no path between start and end points" << endl;
        return false;
    }
    return true;
}

size_t CMap:: manhattan_endpoint_distance ( const pair <size_t, size_t>& pos ) const
{
    return abs ( static_cast<int> ( pos . first ) - static_cast<int> ( m_End . first ) ) + abs ( static_cast<int> ( pos . second ) - static_cast<int> ( m_End . second ) );
}

void CMap:: print_map ( ostream& os ) const
{
    for ( auto& i : m_Map )
    {
        for ( auto& j : i )
            os << j -> m_Icon;
        os << endl;
    }
    os << endl;
}

void CMap:: reconstruct_path ( )
{
    pair <size_t, size_t> path_box = m_Map . at ( m_End . first ) . at ( m_End . second ) -> m_Prev;
    while ( path_box != m_Start )
    {
        m_Map . at ( path_box . first ) . at ( path_box . second ) -> m_Icon = 'o';
        path_box = m_Map . at ( path_box . first ) . at ( path_box . second ) -> m_Prev;
    }
}

bool CMap:: print_results ( ostream& os ) const
{
    print_map ( os );
    os << "--------------------------------------------------------------------------------------------------------" << endl;
    if ( ! read_write_file ( "texts/legend.txt", os ) )
        return false;
    os << "--------------------------------------------------------------------------------------------------------" << endl;
    os << "Nodes expanded: " << m_Box_counter << endl;
    os << "Path length: " << m_Map . at ( m_End . first ) . at ( m_End . second ) -> m_Value << endl;
    return true;
}

bool read_write_file ( const string& file_name, ostream& os )
{
    ifstream file ( file_name );
    if ( ! file . good ( ) )
        return false;
    string line;
    while ( getline ( file, line ) )
    {
        if ( file . fail ( ) )
        {
            file . close ( );
            return false;
        }
        os << line << endl;
    }

    file . close ( );
    return true;
}

bool welcome ( )
{
    if ( ! read_write_file ( "texts/welcome.txt", cout ) )
        return false;

    string start;
    getline ( cin, start );
    if ( cin . bad ( ) )
    {
        cin . clear ( );
        cin . ignore ( streamsize ( -1 ), '\n' );
        return false;
    }
    return true;
}

int select_map ( size_t& id )
{
    if ( ! read_write_file ("texts/select_map.txt", cout ) )
        return -1;
    while ( true )
    {
        char option;
        cin >> option;
        if ( cin . bad ( ) )
        {
            cin . clear ( );
            cin . ignore ( numeric_limits < streamsize > ::max(), '\n' );
            return -1;
        }
        if ( option == 'n' )
            id = ( id + 1 ) % AMOUNT_OF_FILES;
        else if ( option == 'p' )
            id = ( id + AMOUNT_OF_FILES - 1 ) % AMOUNT_OF_FILES;
        else if ( option == 'c' )
            return 1;
        else
        {
            cout << "Invalid operation, please select again." << endl;
            cin . clear ( );
            cin . ignore ( numeric_limits < streamsize > ::max(), '\n' );
            continue;
        }
        break;
    }
    return 0;
}

bool start_map ( size_t& file_id )
{
    while ( true )
    {
        string file_name;
        file_name . append ( DIRECTORY ) . append ( to_string ( file_id ) ) . append ( TXT );
        if ( ! read_write_file ( file_name, cout ) )
            return false;
        int ret_val = select_map ( file_id );
        if ( ret_val == 1 )
           break;
        else if ( ret_val == -1 )
            return false;
    }
    return true;
}

bool select_algorithm ( size_t& algorithm_id )
{
    if ( ! read_write_file ( "texts/algorithms.txt", cout ) )
        return false;
    int option;
    while ( true )
    {
        cin >> option;
        if ( cin . bad ( ) )
        {
            cin . clear ( );
            cin . ignore ( numeric_limits < streamsize > ::max(), '\n' );
            return false;
        }
        if ( ! cin . fail () && option <= 5 && option >= 1 )
            break;
        cout << "Invalid operation, please select again." << endl;
        cin . clear ( );
        cin . ignore ( numeric_limits < streamsize > ::max(), '\n');
    }
    algorithm_id = option;
    return true;
}

bool visual_menu ( bool& till_the_end )
{
    if ( ! read_write_file ( "texts/visualisation_menu.txt", cout ) )
        return false;
    while ( true )
    {
        char option;
        cin >> option;
        if ( cin . bad ( ) )
        {
            cin . clear ( );
            cin . ignore ( numeric_limits < streamsize > ::max(), '\n' );
            return false;
        }
        if ( option == 'p' )
            till_the_end = true;
        else if ( option != 'n' )
        {
            cout << "Invalid operation, please select again." << endl;
            cin . clear ( );
            cin . ignore ( numeric_limits < streamsize > ::max(), '\n' );
            continue;
        }
        break;
    }
    return true;
}


int main ( )
{
    size_t file_id = 0, algorithm_id = 0;
    if ( ! welcome ( ) || ! start_map ( file_id ) || ! select_algorithm ( algorithm_id ) )
        return -1;
    CMap my_map;
    string file_name;
    file_name . append ( DIRECTORY ) . append ( to_string ( file_id ) ) . append ( TXT );
    if ( ! my_map . init_map ( file_name ) )
        return -1;
    cout << endl;
    cout << "Let's find the path" << endl;
    cout << endl;
    if ( ! my_map . run_algorithm ( algorithm_id ) )
        return 1;

    return 0;
}
