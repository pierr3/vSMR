#pragma once

#ifndef DllSpecEuroScope
#define DllSpecEuroScope __declspec ( dllimport )
#define ESINDEX void *
#endif

// external undefined classes
class   CRadarView ;
class   CPlugInData ;

namespace EuroScopePlugIn
{
const   int     COMPATIBILITY_CODE                          = 16 ;

const   int     FLIGHT_PLAN_STATE_NOT_STARTED               = 0 ;
const   int     FLIGHT_PLAN_STATE_SIMULATED                 = 1 ;
const   int     FLIGHT_PLAN_STATE_TERMINATED                = 2 ;

const   int     FLIGHT_PLAN_STATE_NON_CONCERNED             = 0 ;
const   int     FLIGHT_PLAN_STATE_NOTIFIED                  = 1 ;
const   int     FLIGHT_PLAN_STATE_COORDINATED               = 2 ;
const   int     FLIGHT_PLAN_STATE_TRANSFER_TO_ME_INITIATED  = 3 ;
const   int     FLIGHT_PLAN_STATE_TRANSFER_FROM_ME_INITIATED= 4 ;
const   int     FLIGHT_PLAN_STATE_ASSUMED                   = 5 ;
const   int     FLIGHT_PLAN_STATE_REDUNDANT                 = 7 ;

const   int     AIRWAY_CLASS_VALID                          = 0 ;
const   int     AIRWAY_CLASS_DIRECTION_ERROR                = 1 ;
const   int     AIRWAY_CLASS_UNCONNECTED                    = 2 ;
const   int     AIRWAY_CLASS_NO_DATA_DIRECT                 = 3 ;

const   int     CTR_DATA_TYPE_SQUAWK                        = 1 ;
const   int     CTR_DATA_TYPE_FINAL_ALTITUDE                = 2 ;
const   int     CTR_DATA_TYPE_TEMPORARY_ALTITUDE            = 3 ;
const   int     CTR_DATA_TYPE_COMMUNICATION_TYPE            = 4 ;
const   int     CTR_DATA_TYPE_SCRATCH_PAD_STRING            = 5 ;
const   int     CTR_DATA_TYPE_GROUND_STATE                  = 6 ;
const   int     CTR_DATA_TYPE_CLEARENCE_FLAG                = 7 ;
const   int     CTR_DATA_TYPE_DEPARTURE_SEQUENCE            = 8 ;
const   int     CTR_DATA_TYPE_SPEED                         = 9 ;
const   int     CTR_DATA_TYPE_MACH                          = 10 ;
const   int     CTR_DATA_TYPE_RATE                          = 11 ;
const   int     CTR_DATA_TYPE_HEADING                       = 12 ;
const   int     CTR_DATA_TYPE_DIRECT_TO                     = 13 ;

const   int     REFRESH_PHASE_BACK_BITMAP                   = 0 ;
const   int     REFRESH_PHASE_BEFORE_TAGS                   = 1 ;
const   int     REFRESH_PHASE_AFTER_TAGS                    = 2 ;
const   int     REFRESH_PHASE_AFTER_LISTS                   = 3 ;

const   int     TAG_COLOR_DEFAULT                           = 0 ;
const   int     TAG_COLOR_RGB_DEFINED                       = 1 ;
const   int     TAG_COLOR_NON_CONCERNED                     = 2 ;
const   int     TAG_COLOR_NOTIFIED                          = 3 ;
const   int     TAG_COLOR_ASSUMED                           = 4 ;
const   int     TAG_COLOR_TRANSFER_TO_ME_INITIATED          = 5 ;
const   int     TAG_COLOR_REDUNDANT                         = 6 ;
const   int     TAG_COLOR_INFORMATION                       = 7 ;
const   int     TAG_COLOR_ONGOING_REQUEST_FROM_ME           = 8 ;
const   int     TAG_COLOR_ONGOING_REQUEST_TO_ME             = 9 ;
const   int     TAG_COLOR_ONGOING_REQUEST_ACCEPTED          = 10 ;
const   int     TAG_COLOR_ONGOING_REQUEST_REFUSED           = 11 ;
const   int     TAG_COLOR_EMERGENCY                         = 12 ;

const   int     TAG_TYPE_UNTAGGED                       = 0 ;
const   int     TAG_TYPE_TAGGED                         = 1 ;
const   int     TAG_TYPE_DETAILED                       = 2 ;
const   int     TAG_TYPE_TSSR                           = 3 ;

const   int     TAG_ITEM_TYPE_NEXT_LINE                 =  0 ;
const   int     TAG_ITEM_TYPE_STATIC_STRING             =  1 ;
const   int     TAG_ITEM_TYPE_SQUAWK                    =  2 ;
const   int     TAG_ITEM_TYPE_VERTICAL_SPEED_INDICATOR  =  3 ;
const   int     TAG_ITEM_TYPE_ALTITUDE                  =  4 ;
const   int     TAG_ITEM_TYPE_EMERGENCY_INDICATOR       =  5 ;
const   int     TAG_ITEM_TYPE_RADIO_FAILURE_INDICATOR   =  6 ;
const   int     TAG_ITEM_TYPE_HIJACK_INDICATOR          =  7 ;
const   int     TAG_ITEM_TYPE_COLLOSION_ALERT           =  8 ;
const   int     TAG_ITEM_TYPE_CALLSIGN                  =  9 ;
const   int     TAG_ITEM_TYPE_AIRCRAFT_CATEGORY         = 10 ;
const   int     TAG_ITEM_TYPE_COMMUNICATION_TYPE        = 11 ;
const   int     TAG_ITEM_TYPE_VERTICAL_SPEED            = 12 ;
const   int     TAG_ITEM_TYPE_GROUND_SPEED_WITH_N       = 13 ;
const   int     TAG_ITEM_TYPE_HANDOFF_TARGET            = 14 ;
const   int     TAG_ITEM_TYPE_OWNER                     = 15 ;
const   int     TAG_ITEM_TYPE_PLANE_TYPE                = 16 ;
const   int     TAG_ITEM_TYPE_DESTINATION               = 17 ;
const   int     TAG_ITEM_TYPE_SQUAWK_ERROR              = 18 ;
const   int     TAG_ITEM_TYPE_INFO_STRING               = 19 ;
const   int     TAG_ITEM_TYPE_TEMP_ALTITUDE             = 20 ;
const   int     TAG_ITEM_TYPE_INFO_INDICATOR            = 21 ;
const   int     TAG_ITEM_TYPE_FINAL_ALTITUDE            = 22 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_SPEED            = 23 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_RATE             = 24 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_HEADING          = 25 ;
const   int     TAG_ITEM_TYPE_SECTOR_INDICATOR          = 26 ;
const   int     TAG_ITEM_TYPE_DUPLICATED_SQUAWK         = 27 ;
const   int     TAG_ITEM_TYPE_COPN_COPX_NAME            = 28 ;
const   int     TAG_ITEM_TYPE_COPN_COPX_ALTITUDE        = 29 ;
const   int     TAG_ITEM_TYPE_FIR_COPX_NAME             = 30 ;
const   int     TAG_ITEM_TYPE_COPX_NOT_CLEARED_ALTITUDE = 31 ;
const   int     TAG_ITEM_TYPE_COPX_AWERE_TEMP_ALTITUDE  = 32 ;
const   int     TAG_ITEM_TYPE_NEXT_LINE_IF_NOT_EMPTY    = 33 ;
const   int     TAG_ITEM_TYPE_DIRECT                    = 34 ;
const   int     TAG_ITEM_TYPE_GROUND_SPEED_OPTIONAL_WITH_N = 35 ;
const   int     TAG_ITEM_TYPE_FIR_COPX_NAME_OPTIONAL    = 36 ;
const   int     TAG_ITEM_TYPE_DESTINATION_OPTIONAL      = 37 ;
const   int     TAG_ITEM_TYPE_PLANE_TYPE_OPTIONAL       = 38 ;
const   int     TAG_ITEM_TYPE_TSSR                      = 39 ;
const   int     TAG_ITEM_TYPE_GROUND_SPEED_WOUT_N       = 40 ;
const   int     TAG_ITEM_TYPE_GROUND_SPEED_OPTIONAL_WOUT_N = 41 ;
const   int     TAG_ITEM_TYPE_COMPOUND_WARNING          = 42 ;
const   int     TAG_ITEM_TYPE_TEMP_IFSET                = 43 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_SPEED_IFSET      = 44 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_RATE_IFSET       = 45 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_HEADING_IFSET    = 46 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_RUNWAY           = 47 ;
const   int     TAG_ITEM_TYPE_COPN_NAME                 = 48 ;
const   int     TAG_ITEM_TYPE_COPN_ALTITUDE             = 49 ;
const   int     TAG_ITEM_TYPE_COPN_TIME                 = 50 ;
const   int     TAG_ITEM_TYPE_COPX_NAME                 = 51 ;
const   int     TAG_ITEM_TYPE_COPX_ALTITUDE             = 52 ;
const   int     TAG_ITEM_TYPE_COPX_TIME                 = 53 ;
const   int     TAG_ITEM_TYPE_ETA                       = 54 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_STAR             = 55 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_SID              = 56 ;
const   int     TAG_ITEM_TYPE_DEPARTURE_ORDER           = 57 ;
const   int     TAG_ITEM_TYPE_CLEARENCE                 = 58 ;
const   int     TAG_ITEM_TYPE_GROUND_STATUS             = 59 ;
const   int     TAG_ITEM_TYPE_ASSIGNED_SQUAWK           = 60 ;
const   int     TAG_ITEM_TYPE_ORIGIN                    = 61 ;
const   int     TAG_ITEM_TYPE_RVSM_FLAG                 = 62 ;
const   int     TAG_ITEM_TYPE_FLIGHT_RULE               = 63 ;
const   int     TAG_ITEM_TYPE_SECTOR_INDICATOR_FIX      = 64 ;
const   int     TAG_ITEM_TYPE_MANUAL_COORDINATION       = 65 ;
const   int     TAG_ITEM_TYPE_INFO_ALWAYS               = 66 ;
const   int     TAG_ITEM_TYPE_CLAM_WARNING              = 67 ;
const   int     TAG_ITEM_TYPE_RAM_WARNING               = 68 ;
const   int     TAG_ITEM_TYPE_SQ_OR_CALLSIGN            = 69 ;
const   int     TAG_ITEM_TYPE_TWO_LETTER_GS             = 70 ;
const   int     TAG_ITEM_TYPE_TWO_LETTER_GS_OPTIONAL    = 71 ;
const   int     TAG_ITEM_TYPE_TWO_LETTER_ASSIGNED_SPEED = 72 ;
const   int     TAG_ITEM_TYPE_TWO_LETTER_ASSIGNED_SPEED_IFSET = 73 ;
const   int     TAG_ITEM_TYPE_NOT_REACHED_TEMPORARY     = 74 ;
const   int     TAG_ITEM_TYPE_NOT_CLEARED_COPN_COPX_ALT = 75 ;
const   int     TAG_ITEM_TYPE_AIRCRAFT_CATEGORY_WITH_SLASH = 76 ;
const   int     TAG_ITEM_TYPE_NON_RVSM_FLAG             = 77 ;
const   int     TAG_ITEM_TYPE_AC_TYPE_CATEGORY          = 78 ;
const   int     TAG_ITEM_TYPE_AC_TYPE_CATEGORY_OPTIONAL = 79 ;
const   int     TAG_ITEM_TYPE_COMMUNICATION_TYPE_REDUCED= 80 ;
const   int     TAG_ITEM_TYPE_AIRLINE                   = 81 ;
const   int     TAG_ITEM_TYPE_FP_STATUS                 = 82 ;
const   int     TAG_ITEM_TYPE_ESTIMATE                  = 83 ;
const   int     TAG_ITEM_TYPE_ESTIMATE_ALWAYS           = 84 ;
const   int     TAG_ITEM_TYPE_CONFLICTING_AC_CALLSING   = 85 ;
const   int     TAG_ITEM_TYPE_CONFLICT_START            = 86 ;
const   int     TAG_ITEM_TYPE_CONFLICT_END              = 87 ;
const   int     TAG_ITEM_TYPE_CONFLICT_TYPE             = 88 ;
const   int     TAG_ITEM_TYPE_MSAW_INDICATOR            = 89 ;
const   int     TAG_ITEM_TYPE_SIMULATION_INDICATOR      = 90 ;
const   int     TAG_ITEM_TYPE_SIMULATION_WAYPOINT       = 91 ;

const   int     TAG_ITEM_FUNCTION_NO                        = 0 ;
const   int     TAG_ITEM_FUNCTION_TOGGLE_ROUTE_DRAW         = 1 ;
const   int     TAG_ITEM_FUNCTION_TOGGLE_ITEM_DISPLAY       = 2 ;
const   int     TAG_ITEM_FUNCTION_TOGGLE_FIR_COPX_DISPLAY   = 3 ;
const   int     TAG_ITEM_FUNCTION_TOGGLE_DEST_DISPLAY       = 4 ;
const   int     TAG_ITEM_FUNCTION_TOGGLE_PLANE_TYPE_DISPLAY = 5 ;
const   int     TAG_ITEM_FUNCTION_TOGGLE_SI_STYLE           = 6 ;
const   int     TAG_ITEM_FUNCTION_OPEN_FP_DIALOG            = 7 ;
const   int     TAG_ITEM_FUNCTION_HANDOFF_POPUP_MENU        = 8 ;
const   int     TAG_ITEM_FUNCTION_TAKE_HANDOFF              = 9 ;
const   int     TAG_ITEM_FUNCTION_NEXT_ROUTE_POINTS_POPUP   = 10 ;
const   int     TAG_ITEM_FUNCTION_TEMP_ALTITUDE_POPUP       = 11 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_SPEED_POPUP      = 12 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_RATE_POPUP       = 13 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_HEADING_POPUP    = 14 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_MACH_POPUP       = 15 ;
const   int     TAG_ITEM_FUNCTION_TOGGLE_PREDICTION_DRAW    = 16 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_SID              = 17 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_STAR             = 18 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_RUNWAY           = 19 ;
const   int     TAG_ITEM_FUNCTION_ASSIGNED_NEXT_CONTROLLER  = 20 ;
const   int     TAG_ITEM_FUNCTION_COPN_NAME                 = 21 ;
const   int     TAG_ITEM_FUNCTION_COPX_NAME                 = 22 ;
const   int     TAG_ITEM_FUNCTION_COPN_ALTITUDE             = 23 ;
const   int     TAG_ITEM_FUNCTION_COPX_ALTITUDE             = 24 ;
const   int     TAG_ITEM_FUNCTION_ACCEPT_MANUAL_COORDINATION= 25 ;
const   int     TAG_ITEM_FUNCTION_COPN_COPX_ALTITUDE        = 26 ;
const   int     TAG_ITEM_FUNCTION_SET_CLEARED_FLAG          = 27 ;
const   int     TAG_ITEM_FUNCTION_SET_GROUND_STATUS         = 28 ;
const   int     TAG_ITEM_FUNCTION_EDIT_SCRATCH_PAD          = 29 ;
const   int     TAG_ITEM_FUNCTION_RFL_POPUP                 = 30 ;
const   int     TAG_ITEM_FUNCTION_SQUAWK_POPUP              = 31 ;
const   int     TAG_ITEM_FUNCTION_COMMUNICATION_POPUP       = 32 ;
const   int     TAG_ITEM_FUNCTION_CORRELATE_POPUP           = 33 ;
const   int     TAG_ITEM_FUNCTION_SET_FP_STATUS             = 34 ;
const   int     TAG_ITEM_FUNCTION_SET_ESTIMATE              = 35 ;
const   int     TAG_ITEM_FUNCTION_SIMUL_TO_POPUP            = 37 ;
const   int     TAG_ITEM_FUNCTION_SIMUL_LAND_VACATE_POPUP   = 38 ;
const   int     TAG_ITEM_FUNCTION_SIMUL_TAXI_POPUP          = 39 ;
const   int     TAG_ITEM_FUNCTION_SIMUL_TAXI_BEHIND         = 40 ;
const   int     TAG_ITEM_FUNCTION_SIMULATION_POPUP          = 41 ;
const   int     TAG_ITEM_FUNCTION_SIMUL_NEXT_WAYPOINTS      = 42 ;
const   int     TAG_ITEM_FUNCTION_SIMUL_HOLDING_POINTS      = 43 ;

const   int     TAG_DATA_UNCORRELATED_RADAR     = 1 ;
const   int     TAG_DATA_FLIGHT_PLAN_TRACK      = 2 ;
const   int     TAG_DATA_CORRELATED             = 3 ;

const   int     BUTTON_LEFT                 = 1 ;
const   int     BUTTON_MIDDLE               = 2 ;
const   int     BUTTON_RIGHT                = 3 ;

const   int     POPUP_ELEMENT_UNCHECKED     = 0 ;
const   int     POPUP_ELEMENT_CHECKED       = 1 ;
const   int     POPUP_ELEMENT_NO_CHECKBOX   = 2 ;

const   int     CONNECTION_TYPE_NO               = 0 ;
const   int     CONNECTION_TYPE_DIRECT           = 1 ;
const   int     CONNECTION_TYPE_VIA_PROXY        = 2 ;
const   int     CONNECTION_TYPE_SIMULATOR_SERVER = 3 ;
const   int     CONNECTION_TYPE_PLAYBACK         = 4 ;
const   int     CONNECTION_TYPE_SIMULATOR_CLIENT = 5 ;
const   int     CONNECTION_TYPE_SWEATBOX         = 6 ;

const   int     COORDINATION_STATE_NONE                 = 1 ;
const   int     COORDINATION_STATE_REQUESTED_BY_ME      = 2 ;
const   int     COORDINATION_STATE_REQUESTED_BY_OTHER   = 3 ;
const   int     COORDINATION_STATE_ACCEPTED             = 4 ;
const   int     COORDINATION_STATE_REFUSED              = 5 ;
const   int     COORDINATION_STATE_MANUAL_ACCEPTED      = 6 ;

const   int     SECTOR_ELEMENT_INFO                     =  0 ;
const   int     SECTOR_ELEMENT_VOR                      =  1 ;
const   int     SECTOR_ELEMENT_NDB                      =  2 ;
const   int     SECTOR_ELEMENT_AIRPORT                  =  3 ;
const   int     SECTOR_ELEMENT_RUNWAY                   =  4 ;
const   int     SECTOR_ELEMENT_FIX                      =  5 ;
const   int     SECTOR_ELEMENT_STAR                     =  6 ;
const   int     SECTOR_ELEMENT_SID                      =  7 ;
const   int     SECTOR_ELEMENT_LOW_AIRWAY               =  8 ;
const   int     SECTOR_ELEMENT_HIGH_AIRWAY              =  9 ;
const   int     SECTOR_ELEMENT_HIGH_ARTC                = 10 ;
const   int     SECTOR_ELEMENT_ARTC                     = 11 ;
const   int     SECTOR_ELEMENT_LOW_ARTC                 = 12 ;
const   int     SECTOR_ELEMENT_GEO                      = 13 ;
const   int     SECTOR_ELEMENT_FREE_TEXT                = 14 ;
const   int     SECTOR_ELEMENT_AIRSPACE                 = 15 ;
const   int     SECTOR_ELEMENT_POSITION                 = 16 ;
const   int     SECTOR_ELEMENT_SIDS_STARS               = 17 ;
const   int     SECTOR_ELEMENT_RADARS                   = 18 ;
const   int     SECTOR_ELEMENT_REGIONS                  = 19 ;

const   int     SECTOR_ELEMENT_NUMBER                   = 20 ;
const   int     SECTOR_ELEMENT_ALL                      = -1 ;

const   int     RADAR_POSITION_NONE                     = 0 ;
const   int     RADAR_POSITION_PRIMARY                  = 1 ;
const   int     RADAR_POSITION_SECONDARY_C              = 2 ;
const   int     RADAR_POSITION_SECONDARY_S              = 4 ;
const   int     RADAR_POSITION_ALL                      = 7 ;



// forward declaration
class DllSpecEuroScope CSectorElement ;

class DllSpecEuroScope CPosition
{
public:

    double  m_Latitude ;
    double  m_Longitude ;


    //---CPosition-----------------------------------------------------

    inline  CPosition ( void )
    {
        m_Latitude = m_Longitude = 0.0 ;
    } ;


    //---LoadFromStrings-----------------------------------------------

    bool    LoadFromStrings ( const char * sLongitude,
                              const char * sLatitude ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sLongitude  - longitude value
    //      =>  sLatitude   - latitude value
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //
    //  Description :
    //      It loads the position from sectrofile format coordinate strings.
    //-----------------------------------------------------------------


    //---DistanceTo----------------------------------------------------

    double  DistanceTo ( const CPosition OtherPosition ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  OtherPosition   - another position
    //
    //  Return :
    //      the distance between the positions
    //-----------------------------------------------------------------


    //---DirectionTo---------------------------------------------------

    double  DirectionTo ( const CPosition OtherPosition ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  OtherPosition   - another position
    //
    //  Return :
    //      the magnetic direction from this to the other position
    //      using the active sectorfile magnetic deviation value
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CFlightPlanExtractedRoute
{
private:

    ESINDEX     m_FpPosition ;  // the currently referenced AC position

    friend class CFlightPlan ;

public:

    //---CFlightPlanExtractedRoute---------------------------------------

    inline  CFlightPlanExtractedRoute ( void )
    {
        m_FpPosition = NULL ;
    } ;


    //---GetPointsNumber-----------------------------------------------

    int     GetPointsNumber ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The number of points in the extracted route array.
    //-----------------------------------------------------------------


    //---GetPointsCalculatedIndex--------------------------------------

    int     GetPointsCalculatedIndex ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The index of point in the extracted route that edge is the
    //      closest to the actual plane position. The return value is
    //      between 0 and point number - 2.
    //      The value may be -1 indicating invalid state.
    //-----------------------------------------------------------------


    //---GetPointsAssignedIndex----------------------------------------

    int     GetPointsAssignedIndex ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The index of point in the extracted route that was assigned
    //      by a controller as next (direct to). The return value is
    //      between 0 and point number - 1.
    //      The value may be -1 indicating no direct was given.
    //-----------------------------------------------------------------


    //---GetPointName--------------------------------------------------

    const char * GetPointName ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 0 and point number - 1.
    //
    //  Return :
    //      The name of the point in the extracted route array.
    //-----------------------------------------------------------------


    //---GetPointPosition----------------------------------------------

    CPosition    GetPointPosition ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 0 and point number - 1.
    //
    //  Return :
    //      The coordinates of the point in the extracted route array.
    //-----------------------------------------------------------------


    //---GetPointAirwayName--------------------------------------------

    const char * GetPointAirwayName ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 1 and point number - 1.
    //                    For point 0 it is always empty.
    //
    //  Return :
    //      The name of the airway or SID/STAR from the previous point
    //      (Index-1) to this one.
    //-----------------------------------------------------------------


    //---GetPointAirwayClassification----------------------------------

    int     GetPointAirwayClassification ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 1 and point number - 1.
    //                    For point 0 it is always direct to.
    //
    //  Return :
    //      The calssification (see AIRWAY_CLASS_...) of the airway
    //      from the previous point (Index-1) to this one.
    //-----------------------------------------------------------------


    //---GetPointDistanceInMinutes-------------------------------------

    int     GetPointDistanceInMinutes ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 0 and point number - 1.
    //
    //  Return :
    //      The distance of the point in minutes from plane current position.
    //      May return -1 if the point has been passed.
    //-----------------------------------------------------------------


    //---GetPointCalculatedProfileAltitude-----------------------------

    int     GetPointCalculatedProfileAltitude ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 0 and point number - 1.
    //
    //  Return :
    //      The altitude (no FL/altitude change here) calculated from
    //      the route, clib/descend profile and COPX altitude constraints.
    //      The index must be between 0 and point number - 1.
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CFlightPlanPositionPredictions
{
private:

    ESINDEX     m_FpPosition ;  // the currently referenced AC position

    friend class CFlightPlan ;

public:

    //---CFlightPlanPositionPredictions----------------------------------

    inline  CFlightPlanPositionPredictions ( void )
    {
        m_FpPosition = NULL ;
    } ;


    //---GetPointsNumber-----------------------------------------------

    int     GetPointsNumber ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The number of points in the prediction array.
    //-----------------------------------------------------------------


    //---GetPosition---------------------------------------------------

    CPosition   GetPosition ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 0 and point number - 1. The index is
    //                    equal the minutes from now.
    //
    //  Return :
    //      The predicted position of the AC expected at Index minutes from now.
    //-----------------------------------------------------------------


    //---GetAltitude---------------------------------------------------

    int     GetAltitude ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 0 and point number - 1. The index is
    //                    equal the minutes from now.
    //
    //  Return :
    //      The predicted altitude/level of the AC expected at Index
    //      minutes from now. There is no ALT/FL conversion in this
    //      function.
    //-----------------------------------------------------------------


    //---GetControllerId-----------------------------------------------

    const char * GetControllerId ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the requested point. It must be
    //                    between 0 and point number - 1. The index is
    //                    equal the minutes from now.
    //
    //  Return :
    //      The position ID of the predicted controller at Index
    //      minutes from now.
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CRadarTargetPositionData
{
private:

    ESINDEX     m_RtPosition ;  // the currently referenced RT position
    ESINDEX     m_PosPosition ;

    friend class CRadarTarget ;
    friend class CFlightPlan ;

public:

    //---CRadarTargetPositionData-----------------------------------------

    inline  CRadarTargetPositionData ( void )
    {
        m_RtPosition = m_PosPosition = NULL ;
    } ;


    //---IsValid-------------------------------------------------------

    inline  bool    IsValid ( void ) const
    {
        return m_RtPosition != NULL ;
    } ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the position reference is valid
    //      false   - else
    //
    //  Description :
    //      It tests if the AC and the position reference is valid.
    //-----------------------------------------------------------------


    //---IsFPTrackPosition---------------------------------------------

    inline  bool    IsFPTrackPosition ( void ) const
    {
        return m_PosPosition == NULL ;
    } ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the position is a reference to the FP track
    //      false   - else
    //-----------------------------------------------------------------


    //---GetReceivedTime-----------------------------------------------

    int     GetReceivedTime ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The number of seconds elapsed since the position data received.
    //-----------------------------------------------------------------


    //---GetPosition---------------------------------------------------

    CPosition   GetPosition ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The lat/lon coordinates of the plane.
    //-----------------------------------------------------------------


    //---GetSquawk-----------------------------------------------------

    const char * GetSquawk ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The squawk sent by the pilot
    //-----------------------------------------------------------------


    //---GetTransponderC------------------------------------------------

    bool    GetTransponderC ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the plane transponder is C mode
    //      false   - else
    //-----------------------------------------------------------------


    //---GetTransponderI------------------------------------------------

    bool    GetTransponderI ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the plane transponder is IDENT mode
    //      false   - else
    //-----------------------------------------------------------------


    //---GetPressureAltitude-------------------------------------------

    int     GetPressureAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The true altitude of the plane in feet.
    //-----------------------------------------------------------------


    //---GetFlightLevel------------------------------------------------

    int     GetFlightLevel ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The altitude calculated using stadard pressure. The return
    //      value is in feet.
    //-----------------------------------------------------------------


    //---GetReportedGS-------------------------------------------------

    int     GetReportedGS ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The GS reported by the pilot client
    //-----------------------------------------------------------------


    //---GetReportedHeading--------------------------------------------

    int     GetReportedHeading ( void ) const ;
    int     GetReportedHeadingTrueNorth ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The heading (not tracking) reported by the pilot client.
    //-----------------------------------------------------------------


    //---GetReportedPitch----------------------------------------------

    int     GetReportedPitch ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pitch reported by the pilot client. The value is between
    //      -180 and +180; negative means above horizon.
    //-----------------------------------------------------------------


    //---GetReportedBank-----------------------------------------------

    int     GetReportedBank ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pitch reported by the pilot client. The value is between
    //      -180 and +180; negative means right bank.
    //-----------------------------------------------------------------


    //---GetRadarFlags-------------------------------------------------

    int     GetRadarFlags ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The logical or combination of the following values:
    //      - RADAR_POSITION_PRIMARY     - primary position received
    //      - RADAR_POSITION_SECONDARY_C - A+C mode transponder received
    //      - RADAR_POSITION_SECONDARY_S - S mode transponder received
    //
    //  Description :
    //      It returns the radar response flags for the specified position.
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CFlightPlanData
{
private:

    ESINDEX     m_FpPosition ;  // the currently referenced AC position

    friend class CFlightPlan ;

public:

    //---CFlightPlanData-------------------------------------------

    inline  CFlightPlanData ( void )
    {
        m_FpPosition = NULL ;
    } ;


    //---IsReceived----------------------------------------------------

    bool    IsReceived ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if any kind of FP is received from the servers
    //      false   - else
    //-----------------------------------------------------------------


    //---IsAmended-----------------------------------------------------

    bool    IsAmended ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the FP is amended by a controller
    //      false   - else
    //-----------------------------------------------------------------


    //---AmendFlightPlan-----------------------------------------------

    bool    AmendFlightPlan ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
	//		It amends the FP
    //-----------------------------------------------------------------


    //---GetPlanType---------------------------------------------------

    const char * GetPlanType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The type of the flight plan: V or I.
    //-----------------------------------------------------------------


    //---SetPlanType---------------------------------------------------

    bool	SetPlanType ( const char * sPlanType ) ;
    //-----------------------------------------------------------------
	//	Parameters :
	//		=>	sPlanType	- the type of the FP: V or I.
	//
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
	//		It changes the FP type.
    //-----------------------------------------------------------------


    //---GetAircraftInfo-----------------------------------------------

    const char * GetAircraftInfo ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The unextracted aircraft information.
    //-----------------------------------------------------------------


    //---SetAircraftInfo-----------------------------------------------

    bool    SetAircraftInfo ( const char * sInfo ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sInfo   - the complete aircraft information
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
	//		It changes the FP AC info.
    //-----------------------------------------------------------------


    //---GetAircraftWtc------------------------------------------------

    char    GetAircraftWtc ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The weight category of the aircraft.
    //      Possible values are:
    //          ? - unknown
    //          L - light
    //          M - medium
    //          H - heavy
    //          J - super heavy
    //-----------------------------------------------------------------


    //---GetAircraftType-----------------------------------------------

    char    GetAircraftType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The type of the aircraft.
    //      Possible values are:
    //          ? - unknown
    //          L - landplane
    //          S - seaplane
    //          A - amphibian
    //          H - helicopter
    //          G - gyrocopter
    //          T - tilt-wing AC
    //-----------------------------------------------------------------


    //---GetEngineNumber-----------------------------------------------

    int     GetEngineNumber ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The number of engines.
    //-----------------------------------------------------------------


    //---GetEngineType-------------------------------------------------

    char    GetEngineType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The type of the engines of the aircraft.
    //      Possible values are:
    //          ? - unknown
    //          P - piston
    //          T - turboprop/turboshaft
    //          J - jet
    //          E - electric
    //-----------------------------------------------------------------


    //---GetCapibilities-----------------------------------------------

    char    GetCapibilities ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The navigation capibilities of the aircraft.
    //      Possible values are:
    //          ? - unknown
    //          T - no DME, Transponder without mode A+C
    //          X - no DME, No Transponder
    //          U - no DME, Transponder with mode A+C
    //          D - DME, No Transponder
    //          B - DME, Transponder without mode A+C
    //          A - DME, Transponder with mode A+C
    //          M - TACAN only, No Transponder
    //          N - TACAN only, Transponder without mode A+C
    //          P - TACAN only, Transponder with mode A+C
    //          Y - simple RNAV, No Transponder
    //          C - simple RNAV, Transponder without mode A+C
    //          I - simple RNAV, Transponder with mode A+C
    //          E - advanced RNAV with Dual FMS
    //          F - advanced RNAV with Single FMS
    //          G - advanced RNAV with GPS or GNSS
    //          R - advanced RNAV with RNP capability
    //          W - advanced RNAV with RVSM capability
    //          Q - advanced RNAV with RNP and RVSM
    //-----------------------------------------------------------------


    //---IsRvsm--------------------------------------------------------

    bool    IsRvsm ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the aircraft if RVSM capable
    //      false   - else
    //-----------------------------------------------------------------


    //---GetManufacturerType-------------------------------------------

    const char * GetManufacturerType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The type description of the aircraft by the manufacturer.
    //-----------------------------------------------------------------


    //---GetAircraftFPType---------------------------------------------

    const char * GetAircraftFPType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The unencoded AC type as written to the FP.
    //-----------------------------------------------------------------


    //---GetTrueAirspeed-----------------------------------------------

    int     GetTrueAirspeed ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The true airspeed filed.
    //-----------------------------------------------------------------


    //---SetTrueAirspeed-----------------------------------------------

    bool	SetTrueAirspeed ( int TrueAirspeed ) ;
    //-----------------------------------------------------------------
	//  Parameters :
	//      =>  TrueAirspeed - the true airspeed value
	//
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
	//      It sets the true airspeed value.
    //-----------------------------------------------------------------


    //---GetOrigin-----------------------------------------------------

    const char * GetOrigin ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The origin airport.
    //-----------------------------------------------------------------


    //---SetOrigin-----------------------------------------------------

    bool    SetOrigin ( const char * sOrigin ) ;
    //-----------------------------------------------------------------
	//  Parameters :
    //      =>  sOrigin - the origina airport
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It changes the origin airport.
    //-----------------------------------------------------------------


    //---GetFinalAltitude----------------------------------------------

    int     GetFinalAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The final requested altitude.
    //-----------------------------------------------------------------


    //---SetFinalAltitude----------------------------------------------

    bool    SetFinalAltitude ( int FinalAltitude ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FinalAltitude   - the final altitude
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the final altitude.
    //-----------------------------------------------------------------


    //---GetDestination------------------------------------------------

    const char * GetDestination ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The destination airport.
    //-----------------------------------------------------------------


    //---SetDestination------------------------------------------------

    bool    SetDestination ( const char * sDestination ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sDestination    - the destination airport
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the destination airport.
    //-----------------------------------------------------------------


    //---GetAlternate--------------------------------------------------

    const char * GetAlternate ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The alternate airport.
    //-----------------------------------------------------------------


    //---SetAlternate--------------------------------------------------

    bool    SetAlternate ( const char * sAlternate ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      sAlternate  - the alternate airport
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the alternate airport.
    //-----------------------------------------------------------------


    //---GetRemarks----------------------------------------------------

    const char * GetRemarks ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The remarks field.
    //-----------------------------------------------------------------


    //---SetRemarks----------------------------------------------------

    bool    SetRemarks ( const char * sRemarks ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sRemarks    - the remarks field
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the remarks fileds.
    //-----------------------------------------------------------------


    //---GetCommunicationType------------------------------------------

    char    GetCommunicationType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The communications type.
    //      Possible values are:
    //          ? - unknown
    //          V - voice
    //          R - receive voice only
    //          T - text only
    //-----------------------------------------------------------------


    //---GetRoute------------------------------------------------------

    const char * GetRoute ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The route field.
    //-----------------------------------------------------------------


    //---SetRoute------------------------------------------------------

    bool    SetRoute ( const char * sRoute ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sRoute  - the route field
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the route field.
    //-----------------------------------------------------------------


    //---GetSidName----------------------------------------------------

    const char * GetSidName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The extracted or assigned SID name.
    //-----------------------------------------------------------------


    //---GetStarName---------------------------------------------------

    const char * GetStarName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The extracted or assigned STAR name.
    //-----------------------------------------------------------------


    //---GetDepartureRwy-----------------------------------------------

    const char * GetDepartureRwy ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The extracted or assigned departure RWY name.
    //-----------------------------------------------------------------


    //---GetArrivalRwy-------------------------------------------------

    const char * GetArrivalRwy ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The extracted or assigned arrival RWY name.
    //-----------------------------------------------------------------


    //---GetEstimatedDepartureTime-------------------------------------

    const char * GetEstimatedDepartureTime ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pilot sent estimated departure time (uncompiled original
    //      content as string value).
    //-----------------------------------------------------------------


    //---SetEstimatedDepartureTime-------------------------------------

    bool    SetEstimatedDepartureTime ( const char * sDepTime  ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sDepTime    - The estimated departure time (uncompiled
    //                        original content as string value)
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the estimated departure time.
    //-----------------------------------------------------------------


    //---GetActualDepartureTime----------------------------------------

    const char * GetActualDepartureTime ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pilot sent actual departure time (uncompiled original
    //      content as string value).
    //-----------------------------------------------------------------


    //---SetActualDepartureTime----------------------------------------

    bool    SetActualDepartureTime ( const char * sDepTime ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sDepTime    - The actual departure time (uncompiled
    //                        original content as string value)
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the actual departure time.
    //-----------------------------------------------------------------


    //---GetEnrouteHours-----------------------------------------------

    const char * GetEnrouteHours ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pilot sent hours for enroute (uncompiled original
    //      content as string value).
    //-----------------------------------------------------------------


    //---SetEnrouteHours-----------------------------------------------

    bool    SetEnrouteHours ( const char * sEnrouteHours ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sEnrouteHours   - The hours for enroute (uncompiled
    //                            original content as string value)
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the enroute hours field.
    //-----------------------------------------------------------------


    //---GetEnrouteMinutes---------------------------------------------

    const char * GetEnrouteMinutes ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pilot sent minutes for enroute (uncompiled original
    //      content as string value).
    //-----------------------------------------------------------------


    //---SetEnrouteMinutes---------------------------------------------

    bool    SetEnrouteMinutes ( const char * sEnrouteMinutes ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sEnrouteMinutes   - The minutes for enroute (uncompiled
    //                              original content as string value)
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the enroute minutes field.
    //-----------------------------------------------------------------


    //---GetFuelHours--------------------------------------------------

    const char * GetFuelHours ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pilot sent hours available fuel (uncompiled original
    //      content as string value).
    //-----------------------------------------------------------------


    //---SetFuelHours--------------------------------------------------

    bool    SetFuelHours ( const char * sFuelHours ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sFuelHours  - The hours of available fuel (uncompiled
    //                        original content as string value)
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the available fuel hours field.
    //-----------------------------------------------------------------


    //---GetFuelMinutes------------------------------------------------

    const char * GetFuelMinutes ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The pilot sent minutes available fuel (uncompiled original
    //      content as string value).
    //-----------------------------------------------------------------


    //---SetFuelMinutes------------------------------------------------

    bool    SetFuelMinutes ( const char * sFuelMinutes ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sFuelMinutes    - The minutes of available fuel (uncompiled
    //                            original content as string value)
    //
    //  Return :
    //      true    - if success
    //      false   - else
	//
	//	Description :
    //      It sets the available fuel minutes field.
    //-----------------------------------------------------------------


    //---PerformanceGetIas---------------------------------------------

    int     PerformanceGetIas ( int Altitude,
                                int VerticalSpeed ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Altitude        - the actual altitude (or FL)
    //      =>  VerticalSpeed   - VS indicator (-1 for descend,
    //                                           0 for level,
    //                                          +1 for climb )
    //
    //  Return :
    //      The IAS of the AC at the given level and vertical direction
    //-----------------------------------------------------------------


    //---PerformanceGetMach--------------------------------------------

    int     PerformanceGetMach ( int Altitude,
                                 int VerticalSpeed ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Altitude        - the actual altitude (or FL)
    //      =>  VerticalSpeed   - VS indicator (-1 for descend,
    //                                           0 for level,
    //                                          +1 for climb )
    //
    //  Return :
    //      The Mach number (multiplied by 100) of the AC at the given
    //      level and vertical direction
    //-----------------------------------------------------------------


    //---PerformanceGetClimbRate---------------------------------------

    int     PerformanceGetClimbRate ( int Altitude ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Altitude        - the actual altitude (FL)
    //
    //  Return :
    //      The climb rate.
    //-----------------------------------------------------------------


    //---PerformanceGetDescentRate-------------------------------------

    int     PerformanceGetDescentRate ( int Altitude ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Altitude        - the actual altitude (FL)
    //
    //  Return :
    //      The descent rate.
    //-----------------------------------------------------------------

};


class DllSpecEuroScope CFlightPlanControllerAssignedData
{
private:

    ESINDEX     m_FpPosition ;  // the currently referenced AC position

    friend class CFlightPlan ;

public:

    //---CFlightPlanControllerAssignedData-----------------------------

    inline  CFlightPlanControllerAssignedData ( void )
    {
        m_FpPosition = NULL ;
    } ;


    //---GetSquawk-----------------------------------------------------

    const char * GetSquawk ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The assigned squawk (may be empty).
    //-----------------------------------------------------------------


    //---SetSquawk-----------------------------------------------------

    bool    SetSquawk ( const char * sSquawk ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sSquawk - the newly assigned squawk
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetFinalAltitude----------------------------------------------

    int     GetFinalAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The controller ovveriden final altitude. No ALT/FL change here.
    //-----------------------------------------------------------------


    //---SetFinalAltitude----------------------------------------------

    bool    SetFinalAltitude ( int FinalAltitude ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FinalAltitude   - final altitude
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetClearedAltitude--------------------------------------------

    int     GetClearedAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The cleared altitude; no ALT/FL change here. There are
    //      spacial values:
    //          0 - no cleared level (use the final instead of)
    //          1 - cleared for ILS approach
    //          2 - cleared for visual approach
    //-----------------------------------------------------------------


    //---SetClearedAltitude--------------------------------------------

    bool    SetClearedAltitude ( int ClearedAltitude ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  ClearedAltitude - the new cleared altitude (see the
    //                            special values above)
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetCommunicationType------------------------------------------

    char    GetCommunicationType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The communications type.
    //      Possible values are:
    //          0 - unassigned
    //          V - voice
    //          R - receive voice only
    //          T - text only
    //-----------------------------------------------------------------


    //---SetCommunicationType------------------------------------------

    bool    SetCommunicationType ( char CommunicationType ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CommunicationType   - the new type (see the values above)
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetScratchPadString-------------------------------------------

    const char * GetScratchPadString ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The scratch pad string value. Only the real valus, directs,
    //      assigned speed, heading etc are not stored here.
    //-----------------------------------------------------------------


    //---SetScratchPadString-------------------------------------------

    bool    SetScratchPadString ( const char * sString ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sString - the new scratch string value
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetAssignedSpeed----------------------------------------------

    int     GetAssignedSpeed ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The controller assigned speed. Value 0 indicates no assignment.
    //-----------------------------------------------------------------


    //---SetAssignedSpeed----------------------------------------------

    bool    SetAssignedSpeed ( int AssignedSpeed ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  AssignedSpeed   - the new assigned speed
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetAssignedMach-----------------------------------------------

    int     GetAssignedMach ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The controller assigned mach number multiplied by 100.
    //      750 indicates 0.75 Mach. Value 0 indicates no assignment.
    //-----------------------------------------------------------------


    //---SetAssignedMach-----------------------------------------------

    bool    SetAssignedMach ( int AssignedMach ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  AssignedMach    - the assigned Mach number (multiplied
    //                            by 100)
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetAssignedRate-----------------------------------------------

    int     GetAssignedRate ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The controller assigned climb/descend rate. There is no
    //      indication if climb or descend rate is assigned. Value 0
    //      indicates no assignment.
    //-----------------------------------------------------------------


    //---SetAssignedRate-----------------------------------------------

    bool    SetAssignedRate ( int AssignedRate ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  AssignedRate    - the assigned rate
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetAssignedHeading--------------------------------------------

    int     GetAssignedHeading ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The controller assigned heading. Value 0 indicates no assignment.
    //-----------------------------------------------------------------


    //---SetAssignedHeading--------------------------------------------

    bool    SetAssignedHeading ( int AssignedHeading  ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  AssignedHeading - assigned heading
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetDirectToPointName------------------------------------------

    const char * GetDirectToPointName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The assigned direct to point name.
    //-----------------------------------------------------------------


    //---SetDirectToPointName------------------------------------------

    bool    SetDirectToPointName ( const char * sPointName ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sPointName  - the name of the point
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------


    //---GetFlightStripAnnotation--------------------------------------

    const char * GetFlightStripAnnotation ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index   - the index of the annotation (0-8).
    //
    //  Return :
    //      The selected fligt strip annotation.
    //-----------------------------------------------------------------


    //---SetFlightStripAnnotation--------------------------------------

    bool    SetFlightStripAnnotation ( int Index,
                                       const char * sAnnotation ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index       - the index of the annotation (0-8).
    //      =>  sAnnotation - the text itself
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CFlightPlan
{
private:

    ESINDEX     m_FpPosition ;  // the currently referenced AC position

    friend  class   CPlugInData ;
    friend  class   CPlugIn ;
    friend  class   CFlightPlanList ;
    friend  class   CRadarTarget ;

public:

    //---CFlightPlan---------------------------------------------------

    inline  CFlightPlan ( void )
    {
        m_FpPosition = NULL ;
    } ;


    //---IsValid-------------------------------------------------------

    inline  bool    IsValid ( void ) const
    {
        return m_FpPosition != NULL ;
    } ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the AC reference is valid
    //      false   - else
    //
    //  Description :
    //      It tests if the AC reference is valid.
    //-----------------------------------------------------------------


    //---GetCallsign---------------------------------------------------

    const char * GetCallsign ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The callsign of the AC.
    //-----------------------------------------------------------------


    //---GetPilotName--------------------------------------------------

    const char * GetPilotName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The name of the pilot extracted from statistics data.
    //      Empty string if no selected AC or no statistics data
    //-----------------------------------------------------------------


    //---GetState------------------------------------------------------

    int     GetState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state of the AC (see FLIGHT_PLAN_STATE_ ...).
    //      0 if no selected AC
    //-----------------------------------------------------------------


    //---GetFPState----------------------------------------------------

    int     GetFPState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state of the AC (FLIGHT_PLAN_STATE_NOT_STARTED,
    //                           FLIGHT_PLAN_STATE_SIMULATED,
    //                           FLIGHT_PLAN_STATE_TERMINATED)
    //      0 if no selected AC
    //-----------------------------------------------------------------


    //---GetSimulated--------------------------------------------------

    bool    GetSimulated ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the AC is out of range and ES simulates its movements
    //      false   - else
    //-----------------------------------------------------------------


    //---GetTrackingControllerCallsign---------------------------------

    const char * GetTrackingControllerCallsign ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The callsign of the controller currently tracking.
    //      Empty if noone is tracking it
    //-----------------------------------------------------------------


    //---GetTrackingControllerId---------------------------------------

    const char * GetTrackingControllerId ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The position ID of the controller currently tracking.
    //      Empty if noone is tracking it
    //-----------------------------------------------------------------


    //---GetTrackingControllerIsMe-------------------------------------

    bool    GetTrackingControllerIsMe ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the plane is tracked by me
    //      false   - else
    //-----------------------------------------------------------------


    //---GetHandoffTargetControllerCallsign----------------------------

    const char * GetHandoffTargetControllerCallsign ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The callsign of the controller who is the target of the
    //          actual handoff.
    //      Empty if no actual handoff.
    //-----------------------------------------------------------------


    //---GetHandoffTargetControllerId----------------------------------

    const char * GetHandoffTargetControllerId ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The position ID of the controller who is the target of the
    //          actual handoff.
    //      Empty if no actual handoff.
    //-----------------------------------------------------------------


    //---GetDistanceToDestination--------------------------------------

    double  GetDistanceToDestination ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The full calculated distance to the destination airport.
    //-----------------------------------------------------------------


    //---GetDistanceFromOrigin-----------------------------------------

    double  GetDistanceFromOrigin ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The full calculated distance from the origin airport.
    //-----------------------------------------------------------------


    //---GetNextCopxPointName------------------------------------------

    const char * GetNextCopxPointName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The next active COPX point name along the extracted route.
    //      May be empty.
    //-----------------------------------------------------------------


    //---GetNextFirCopxPointName---------------------------------------

    const char * GetNextFirCopxPointName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The next active FIR COPX point name along the extracted route.
    //      May be empty.
    //-----------------------------------------------------------------


    //---GetSectorEntryMinutes-----------------------------------------

    int     GetSectorEntryMinutes ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The time in minutes this plane will enter to my sectors.
    //      If already inside then 0 will be returned.
    //      If it will never enter then -1 will be returned.
    //-----------------------------------------------------------------


    //---GetSectorExitMinutes------------------------------------------

    int     GetSectorExitMinutes ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The time in minutes this plane will leave to my sectors.
    //      If the plane will never enter to my sectors it will ne -1.
    //-----------------------------------------------------------------


    //---GetRAMFlag----------------------------------------------------

    bool    GetRAMFlag ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The RAM flag when RAM warning is to be displayed.
    //-----------------------------------------------------------------


    //---GetCLAMFlag---------------------------------------------------

    bool    GetCLAMFlag ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The CLAM flag when CLAM warning is to be displayed.
    //-----------------------------------------------------------------


    //---GetGroundState------------------------------------------------

    const char * GetGroundState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The ground state of the AC (PUSH, TAXI, DEPA or empty).
    //-----------------------------------------------------------------


    //---GetClearenceFlag----------------------------------------------

    bool    GetClearenceFlag ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state if the clearence is received or not.
    //-----------------------------------------------------------------


    //---IsTextCommunication-------------------------------------------

    bool    IsTextCommunication ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the pilot or a controller indicated
    //      text communication.
    //-----------------------------------------------------------------


    //---GetFinalAltitude----------------------------------------------

    int     GetFinalAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The final altitude from the FP or from controller override.
    //      There is no ALT/FL change here.
    //-----------------------------------------------------------------


    //---GetClearedAltitude--------------------------------------------

    int     GetClearedAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The cleared altitude. If missing then the final one.
    //      There is no ALT/FL change here.
    //-----------------------------------------------------------------


    //---GetEntryCoordinationPointState--------------------------------

    int     GetEntryCoordinationPointState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state of the entry point coordination. One of the
    //      COORDINATION_STATE_...
    //-----------------------------------------------------------------


    //---GetEntryCoordinationPointName---------------------------------

    const char *    GetEntryCoordinationPointName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The cordination point name for entry point.
    //-----------------------------------------------------------------


    //---GetEntryCoordinationAltitudeState-----------------------------

    int     GetEntryCoordinationAltitudeState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state of the entry point altitude coordination. One of the
    //      COORDINATION_STATE_...
    //-----------------------------------------------------------------


    //---GetEntryCoordinationAltitude----------------------------------

    int     GetEntryCoordinationAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The cordination point altitude for entry point.
    //-----------------------------------------------------------------


    //---GetExitCoordinationNameState----------------------------------

    int     GetExitCoordinationNameState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state of the exit point name coordination. One of the
    //      COORDINATION_STATE_...
    //-----------------------------------------------------------------


    //---GetExitCoordinationPointName----------------------------------

    const char *    GetExitCoordinationPointName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The cordination point name for exit point.
    //-----------------------------------------------------------------


    //---GetExitCoordinationAltitudeState------------------------------

    int     GetExitCoordinationAltitudeState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state of the exit point altitude coordination. One of the
    //      COORDINATION_STATE_...
    //-----------------------------------------------------------------


    //---GetExitCoordinationAltitude-----------------------------------

    int     GetExitCoordinationAltitude ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The cordination point altitude for entry point.
    //-----------------------------------------------------------------


    //---GetCoordinatedNextController----------------------------------

    const char * GetCoordinatedNextController ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      - the callsign of the next controller if there is a
    //        coordinated one
    //      - empty else
    //-----------------------------------------------------------------


    //---GetCoordinatedNextControllerState-----------------------------

    int     GetCoordinatedNextControllerState ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The state of the next controller coordination. One of the
    //      COORDINATION_STATE_...
    //-----------------------------------------------------------------


    //---GetCorrelatedRadarTarget--------------------------------------

    CRadarTarget    GetCorrelatedRadarTarget ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The correlated radar target. If no correlated target then
    //      the returned element is invalid.
    //-----------------------------------------------------------------


    //---CorrelateWithRadarTarget--------------------------------------

    bool    CorrelateWithRadarTarget ( CRadarTarget RadarTarget ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  RadarTarget - the radar target to get correlated wit
    //
    //  Retrun :
    //      true    - if success
    //      false   - if failed
    //
    //  Description :
    //      Call this function to correlate an flight plan with a radar
    //      target. Afetr calling it you DO NOT need to call the
    //      CorrelateWithFlightPlan on the radar target.
    //-----------------------------------------------------------------


    //---Uncorrelate---------------------------------------------------

    void    Uncorrelate ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It uncorrelates the flight plan from its current target.
    //      After uncorrelate you DO NOT need to call the Uncorrelate
    //      on the radar target position. After uncorrelating the FP
    //      will not be correlated automatically by the system
    //-----------------------------------------------------------------


    //---StartTracking-------------------------------------------------

    bool    StartTracking ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if success
    //      false   - else
    //
    //  Description :
    //      It starts tracking the AC if it was untracked before.
    //-----------------------------------------------------------------


    //---EndTracking---------------------------------------------------

    bool    EndTracking ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if success
    //      false   - else
    //
    //  Description :
    //      It releases (drops) the target.
    //-----------------------------------------------------------------


    //---InitiateHandoff-----------------------------------------------

    bool    InitiateHandoff ( const char * sTargetController ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sTargetController   - the controller to initiate the
    //                                handoff to
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //
    //  Description :
    //      It initiates a handoff to the specified target controller.
    //-----------------------------------------------------------------


    //---AcceptHandoff-------------------------------------------------

    void    AcceptHandoff ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It accepts a handoff initiated to me.
    //-----------------------------------------------------------------


    //---RefuseHandoff-------------------------------------------------

    void    RefuseHandoff ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It refuses a handoff initiated to me.
    //-----------------------------------------------------------------


    //---InitiateCoordination------------------------------------------

    bool    InitiateCoordination ( const char * sTargetController,
                                   const char * sPointName,
                                   int Altitude ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sTargetController   - the controller to initiate the
    //                                handoff to
    //      =>  sPointName          - the requested point name (it
    //                                can not be empty)
    //      =>  Altitude            - the requested altitude (pass 0
    //                                if only the point is requested)
    //
    //  Return :
    //      true    - if success
    //      false   - else
    //
    //  Description :
    //      It initiates a COPN/COPX request.
    //-----------------------------------------------------------------


    //---AcceptCoordination--------------------------------------------

    void    AcceptCoordination ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It accepts a coordination initiated to me.
    //-----------------------------------------------------------------


    //---RefuseCoordination--------------------------------------------

    void    RefuseCoordination ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It refuses a handoff initiated to me.
    //-----------------------------------------------------------------


    //---PushFlightStrip-----------------------------------------------

    void    PushFlightStrip ( const char * sTargetController ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sTargetController   - the controller to send the flight
    //                                strip to
    //-----------------------------------------------------------------


    //---SetEstimation-------------------------------------------------

    void    SetEstimation ( const char * sPointName,
                            const char * sTime ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sPointName  - the name along the route
    //      =>  sTime       - the estimated arrival time in HHMM format
    //                        in zulu time.
    //-----------------------------------------------------------------


    //---ClearEstimation-----------------------------------------------

    void    ClearEstimation ( void ) ;
    void    ClearEstimation ( const char * sPointName ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sPointName  - the name along the route
    //
    //  Description :
    //      It clears the estimated arrival time from the given point.
    //      The first release clears all estimations.
    //-----------------------------------------------------------------


    //---GetExtractedRoute---------------------------------------------

    CFlightPlanExtractedRoute GetExtractedRoute ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      An extracted route object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //-----------------------------------------------------------------


    //---GetPositionPredictions----------------------------------------

    CFlightPlanPositionPredictions    GetPositionPredictions ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A position prediction array object instance.
    //      The position prediction array contains the position used
    //      by ES to determinate where the AC will be at a given time.
    //      Every element in the array contains data for the position
    //      flown by the next minute. Therefore the size of the array is
    //      always the remaining flight time in minutes.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //-----------------------------------------------------------------


    //---GetFPTrackPosition--------------------------------------------

    CRadarTargetPositionData   GetFPTrackPosition ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flight plan track position. This position data points to the
    //      FP track position and can not be used for subsequent 
    //      GetPreviousPosition calls.
    //      The FP position is the same as the last radar target position
    //      if the FP is correlated. Otherwise the system calculated position.
    //-----------------------------------------------------------------


    //---GetFlightPlanData---------------------------------------------

    CFlightPlanData GetFlightPlanData ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flight plan data object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //-----------------------------------------------------------------


    //---GetControllerAssignedData-------------------------------------

    CFlightPlanControllerAssignedData   GetControllerAssignedData ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      An instance that holds controller assigned data.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CRadarTarget
{
private:

    ESINDEX     m_RtPosition ;  // the currently referenced AC position

    friend  class   CPlugInData ;
    friend  class   CPlugIn ;
    friend  class   CFlightPlan ;

public:

    //---CRadarTarget--------------------------------------------------

    inline  CRadarTarget ( void )
    {
        m_RtPosition = NULL ;
    } ;


    //---IsValid-------------------------------------------------------

    inline  bool    IsValid ( void ) const
    {
        return m_RtPosition != NULL ;
    } ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the AC reference is valid
    //      false   - else
    //
    //  Description :
    //      It tests if the AC reference is valid.
    //-----------------------------------------------------------------


    //---GetCallsign---------------------------------------------------

    const char * GetCallsign ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The callsign of the AC.
    //-----------------------------------------------------------------


    //---GetSystemID---------------------------------------------------

    const char * GetSystemID ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The system assigned ID to the radar target. It is surely the
    //      same on all instances - as the code itself is generated from
    //      the callsign.
    //-----------------------------------------------------------------


    //---GetVerticalSpeed----------------------------------------------

    int     GetVerticalSpeed ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The calculated vertical speed of the selected AC in feet/minute.
    //          Be careful, it might be extremely inaccurate due the
    //          protocol. Use only for climb/descend/level test
    //      0   - else
    //-----------------------------------------------------------------


    //---GetTrackHeading-----------------------------------------------

    double  GetTrackHeading ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The calculated track direction in degree
    //-----------------------------------------------------------------


    //---GetGS---------------------------------------------------------

    int     GetGS ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The ground speed. First it uses the plane reported GS. If
    //      that is 0 then uses the calculated one.
    //-----------------------------------------------------------------


    //---GetCorrelatedFlightPlan---------------------------------------

    CFlightPlan GetCorrelatedFlightPlan ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The currently correlated flight plan. If the radar target
    //      is uncorrelated then the returned flight plan is invalid.
    //-----------------------------------------------------------------


    //---CorrelateWithFlightPlan---------------------------------------

    bool    CorrelateWithFlightPlan ( CFlightPlan FlightPlan ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the flight plan to be correlated with
    //
    //  Retrun :
    //      true    - if success
    //      false   - if failed
    //
    //  Description :
    //      Call this function to correlate an radar target with a
    //      flight plan. After this function you DO NOT need to call the
    //      CorrelateWithradarTarget on the flight plan.
    //-----------------------------------------------------------------


    //---Uncorrelate---------------------------------------------------

    void    Uncorrelate ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It uncorrelates the radar target from the flight plan.
    //      After it you DO NOT need to call the Uncorrelate on the
    //      flight plan. After uncorrelating the system will never
    //      correlate it automatically.
    //-----------------------------------------------------------------


    //---GetPosition---------------------------------------------------

    CRadarTargetPositionData   GetPosition ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A plane position array instance. After calling this function
    //      the returned variable points to the last position of the radar target.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //-----------------------------------------------------------------


    //---GetPreviousPosition-------------------------------------------

    CRadarTargetPositionData   GetPreviousPosition ( const CRadarTargetPositionData CurrentPosition ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CurrentPosition - the current position object
    //
    //  Return :
    //      A plane position object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //-----------------------------------------------------------------


} ;


class DllSpecEuroScope CController
{
private:

    ESINDEX     m_CtrPosition ;  // the currently referenced AC position
    bool        m_Myself ;               // indicates myself is selected

    friend  class   CPlugInData ;
    friend  class   CPlugIn ;

public:

    //---CController---------------------------------------------------

    inline  CController ( void )
    {
        m_CtrPosition = NULL ;
        m_Myself      = false ;
    } ;


    //---IsValid-------------------------------------------------------

    inline  bool    IsValid ( void ) const
    {
        return m_CtrPosition != NULL || m_Myself ;
    } ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the CTR reference is valid
    //      false   - else
    //
    //  Description :
    //      It tests if the CTR reference is valid.
    //-----------------------------------------------------------------


    //---GetCallsign---------------------------------------------------

    const char * GetCallsign ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The callsign of the controller.
    //-----------------------------------------------------------------


    //---GetPositionId-------------------------------------------------

    const char * GetPositionId ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The position ID of the selected controller.
    //-----------------------------------------------------------------


    //---GetPositionIdentified-----------------------------------------

    bool    GetPositionIdentified ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the controller is identified in the position file
    //      false   - else
    //-----------------------------------------------------------------


    //---GetPrimaryFrequency-------------------------------------------

    double  GetPrimaryFrequency ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      the primary frequency of the controller
    //      199.980 if no primary frequency is selected
    //-----------------------------------------------------------------


    //---GetFullName---------------------------------------------------

    const char * GetFullName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The position ID of the selected controller.
    //-----------------------------------------------------------------


    //---GetRating-----------------------------------------------------

    int     GetRating ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      the rating of the controller:
    //          1   - observer, OBS
    //          2   - old STU or student 1, now Tower (TWR)
    //          3   - old student 2 - never used
    //          4   - old STU+ or student 3, now Approach (APP)
    //          5   - old CTR or controller 1, now Centre (CTR)
    //          6   - old controller 2 - never used
    //          7   - old CTR+ or controller 3, now Senior Controller (CTR+)
    //          8   - instructor 1, INS
    //          9   - instructor 2 - never used
    //         10   - instructor 3, INS+
    //         11   - supervisor, SUP
    //         12   - administrator, ADM
    //-----------------------------------------------------------------


    //---GetFacility---------------------------------------------------

    int     GetFacility ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      the facility of the controller:
    //          1   - FSS - flight service station
    //          2   - DEL - delivery
    //          3   - GND - ground
    //          4   - TWR - tower
    //          5   - APP, DEP - approach, departure
    //          6   - CTR - controller
    //-----------------------------------------------------------------


    //---GetSectorFileName---------------------------------------------

    const char * GetSectorFileName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The name of the sectorfile used by the controller.
    //-----------------------------------------------------------------


    //---IsController--------------------------------------------------

    bool    IsController ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the controller is accepted as controller by the
    //                servers (it has right to track AC, change FP, etc).
    //      false   - else
    //-----------------------------------------------------------------


    //---GetPosition---------------------------------------------------

    CPosition   GetPosition ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The center position of the controller. It returns only the
    //      main center but never the additional visibility points.
    //-----------------------------------------------------------------


    //---GetRange------------------------------------------------------

    int     GetRange ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The visibility range of the controller.
    //-----------------------------------------------------------------


    //---IsBreaking----------------------------------------------------

    bool    IsBreaking ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      The breaking state of the controller.
    //-----------------------------------------------------------------


    //---IsOngoingAble-------------------------------------------------

    bool    IsOngoingAble ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      Indicates if ready for ongoing coordination (indicates
    //      EuroScope on the other side).
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CRadarScreen
{
private:

    CRadarView  * m_pRadarView ;     // pointer to the structure behind
    CPlugIn     * m_pPlugIn ;
                          // pointer back to the plugin for easy access

    friend CPlugInData ;

public:

    //---CRadarScreen--------------------------------------------------

    CRadarScreen ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      The default constructor.
    //-----------------------------------------------------------------


    //---GetPlugIn-----------------------------------------------------

    inline  CPlugIn * GetPlugIn ( void )
    {
        return m_pPlugIn ;
    } ;


    //---GetRadarView--------------------------------------------------

    inline  CRadarView * GetRadarView ( void )
    {
        return m_pRadarView ;
    } ;


    //---GetToolbarArea------------------------------------------------

    RECT    GetToolbarArea ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The rectangle that is cobered by the toolbar in pixels.
    //      This are is not used for radar display.
    //-----------------------------------------------------------------


    //---GetRadarArea--------------------------------------------------

    RECT    GetRadarArea ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The rectangle that is covered by the radar screen.
    //-----------------------------------------------------------------


    //---GetChatArea---------------------------------------------------

    RECT    GetChatArea ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The rectangle that is covered by the chat lines.
    //-----------------------------------------------------------------


    //---ConvertCoordFromPixelToPosition-------------------------------

    CPosition   ConvertCoordFromPixelToPosition ( POINT Pt ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Pt      - a pixel coordinates
    //
    //  Return :
    //      The value converted to Lat, Lon position.
    //-----------------------------------------------------------------


    //---ConvertCoordFromPositionToPixel-------------------------------

    POINT   ConvertCoordFromPositionToPixel ( CPosition Pos ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Pos     - a lat, lon position
    //
    //  Return :
    //      The value converted to pixels.
    //-----------------------------------------------------------------


    //---SaveDataToAsr-------------------------------------------------

    void    SaveDataToAsr ( const char * sVariableName,
                            const char * sVariableDescription,
                            const char * sValue ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sVariableName       - the name under your value will be saved
    //      =>  sVariableDescription- a user readable name of the variable;
    //                                this value will be shown in the
    //                                save modified settings dialog; if
    //                                empty (never pass NULL), then it
    //                                will not be shown
    //      =>  sValue              - the value itself to be stored; it
    //                                may contain only printable chars and
    //                                never LF or CR or ':'.
    //
    //  Description :
    //      It saves the value for the plug in the ASR file.
    //-----------------------------------------------------------------


    //---GetDataFromAsr------------------------------------------------

    const char  * GetDataFromAsr ( const char * sVariableName ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sVariableName       - the name under your value was saved
    //
    //  Return :
    //      the value found in the ASR or
    //      NULL if not found
    //
    //  Description :
    //      It loads the value for the plug from the ASR file.
    //-----------------------------------------------------------------


    //---AddScreenObject-----------------------------------------------

    void    AddScreenObject ( int ObjectType,
                              const char * sObjectId,
                              RECT Area,
                              bool Moveable,
                              const char * sMessage ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  ObjectType  - the type of the object (depends on the plugin)
    //      =>  sObjectId   - the object identifier (most commin is to set
    //                        the callisgn or similar)
    //      =>  Area        - the area covered by the screen object
    //      =>  Moveable    - indicates if the user can move it
    //      =>  sMessage    - a message to be diplayed in the message
    //                        area of the main screen when the obejct
    //                        is clicked
    //
    //  Description :
    //      This is a really short living object. Define all clickable
    //      areas on the radar screen when drawing your graphics
    //      elements. Call this function from the OnRefresh function
    //      only. The object list is cleared before the next refresh.
    //-----------------------------------------------------------------


    //---RequestRefresh------------------------------------------------

    void    RequestRefresh ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      Call this function when you need an update on the screen.
    //      It just posts a request and the screen will be painted
    //      only once when the thread finished all other tasks. Do
    //      call when the content is changed and it is absolutely
    //      necessary to refresj the screen before the standard 1 sec
    //      refresh.
    //-----------------------------------------------------------------


    //---ShowSectorFileElement-----------------------------------------

    void    ShowSectorFileElement ( CSectorElement Element,
                                    const char * sComponentName,
                                    bool Show ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Element         - the element to be shown/hidden
    //      =>  sComponentName  - the displayable component name (call
    //                            the GetComponentName to retreive
    //                            the available component names)
    //      =>  Show            - true to show, false to hide
    //
    //  Description :
    //      This function shows or hides individual sector file elements
    //      on the screen. This function just saves the flag, but does
    //      not refresh the background map as it takes long time. When
    //      you finished all switching call the RefreshMapContent to
    //      really refresh the elements.
    //-----------------------------------------------------------------


    //---RefreshMapContent---------------------------------------------

    void    RefreshMapContent ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      This function refreshes the background map content of the view.
    //      Call this function after setting all the necessary elements
    //      on and off.
    //-----------------------------------------------------------------


    //---StartTagFunction----------------------------------------------

    void    StartTagFunction ( const char * sCallsign,
                               const char * sItemPlugInName,
                               int ItemCode,
                               const char * sItemString,
                               const char * sFunctionPlugInName,
                               int FunctionId,
                               POINT Pt,
                               RECT Area ) ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  sCallsign           - the AC which TAG is clicked
    //      =>  sItemPlugInName     - the item provider plug-in (for base
    //                                EuroScope TAG functions pass NULL)
    //      =>  ItemCode            - the item code
    //      =>  sItemString         - the string of the selected item (as
    //                                these are TAG item functions they may
    //                                require the actual item string)
    //      =>  sFunctionPlugInName - the function provider plugin (for base
    //                                EuroScope TAG functions pass NULL)
    //      =>  FunctionId          - the ID of the function
    //      =>  Pt                  - the mouse position
    //      =>  Area                - the area covered by the TAG item
    //
    //  Description :
    //      Call this function to execute the inner logic of EuroScope
    //      after clicking on a TAG item. Be sure to ASEL the appropriate
    //      AC before initiating the TAG function as most of them
    //      operates on the ASEL one.
    //-----------------------------------------------------------------


    //---GetDisplayArea------------------------------------------------

    void    GetDisplayArea ( CPosition * pLeftDown,
                             CPosition * pRightUp ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      <=  pLeftDown   - the left down corner of the screen
    //      <=  pRightUp    - the right up corner of the screen
    //
    //  Description :
    //      It returns the position of the left-down and right-up corners.
    //-----------------------------------------------------------------


    //---SetDisplayArea------------------------------------------------

    void    SetDisplayArea ( CPosition LeftDown,
                             CPosition RightUp ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  LeftDown    - the desired left down corner of the screen
    //      =>  RightUp     - the desired right up corner of the screen
    //
    //  Description :
    //      It update the display area of the screen to fully cover
    //      the defined rectangle. The are might bi bigger if the
    //      aspect ratio of the screen is different.
    //-----------------------------------------------------------------


    //---OnAsrContentLoaded--------------------------------------------

    inline  virtual void    OnAsrContentLoaded ( bool Loaded )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  Loaded  - indicates if the content was really loaded
    //                    from a file (true) or the ASR file is ready
    //                    just after NEW command.
    //
    //  Description :
    //      This function is called after the initialization of the
    //      ASR file is complete. It may be after loading it from the
    //      file or just after executing a NEW command.
    //------------------------------------------------------------------


    //---OnAsrContentToBeSaved------------------------------------------

    virtual void    OnAsrContentToBeSaved ( void )
    {
    } ;
    //------------------------------------------------------------------
    //  Description :
    //      This function is called just before the content change dialog.
    //      is saved. It is the latest time you can register any change
    //      to the ASR file to be prompted to save or not.
    //------------------------------------------------------------------


    //---OnRefresh------------------------------------------------------

    inline  virtual void    OnRefresh ( HDC hDC,
                                        int Phase )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  hDC     - handle to the device context
    //      =>  Phase   - the refresh phase
    //
    //  Description :
    //      This function is called several times during the refresh. The
    //      Phase indicates the actual state:
    //          REFRESH_PHASE_BACK_BITMAP   - used to draw the content of
    //                                        background bitmap, it is updated
    //                                        only on zooming/panning
    //          REFRESH_PHASE_BEFORE_TAGS   - before TAGs are displayed
    //          REFRESH_PHASE_AFTER_TAGS    - after TAGs, before lists
    //          REFRESH_PHASE_AFTER_LISTS   - after all (before chat,
    //                                        toolbar and popup menus).
    //      The hDC points to an internal bitmap. Therefore your drawings
    //      will not be displayed immediately. Only when all drawings are
    //      ready the bitmap is copied to the screen.
    //      Normally this function is called once a second. Be fast and
    //      efficient to draw all your drawings in a resonable short time.
    //      When something is dragged on the screen this function will be
    //      called far more often.
    //      In the current version there is no way to refresh only part
    //      of the radar screen. It is painted as whole all the times.
    //      Therefore a fast drawing is really necessary.
    //------------------------------------------------------------------


    //---OnAsrContentToBeClosed------------------------------------------

    virtual void    OnAsrContentToBeClosed ( void ) = NULL ;
    //------------------------------------------------------------------
    //  Description :
    //      This function is called just before close of the ASR file.
    //      Actually you can not save data here, but you may clean up
    //      things.
    //      IMPORTANT: You MUST free the instance as the last action
    //      of this callback. Call: delete this.
    //------------------------------------------------------------------


    //---OnControllerPositionUpdate------------------------------------

    inline  virtual void    OnControllerPositionUpdate ( CController Controller )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Controller  - the controller reference whose position
    //                        is updated
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a controller position is updated.
    //-----------------------------------------------------------------


    //---OnControllerDisconnect----------------------------------------

    inline  virtual void    OnControllerDisconnect ( CController Controller )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Controller  - the controller reference
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a controller is logged off or its position
    //      update is timed out.
    //-----------------------------------------------------------------


    //---OnRadarTargetPositionUpdate--------------------------------------

    inline  virtual void    OnRadarTargetPositionUpdate ( CRadarTarget RadarTarget )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  RadarTarget - the radara reference whose position
    //                        is updated
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a radar target position is updated.
    //-----------------------------------------------------------------


    //---OnFlightPlanDisconnect----------------------------------------

    inline  virtual void    OnFlightPlanDisconnect ( CFlightPlan FlightPlan )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the flight plan reference
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a flight plan is logged off or its position
    //      update is timed out.
    //-----------------------------------------------------------------


    //---OnFlightPlanFlightPlanDataUpdate------------------------------

    inline  virtual void    OnFlightPlanFlightPlanDataUpdate ( CFlightPlan FlightPlan )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the flight plan reference whose flight plan
    //                        data is updated
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a flight plan flight plan data is updated.
    //-----------------------------------------------------------------


    //---OnFlightPlanControllerAssignedDataUpdate----------------------

    inline  virtual void    OnFlightPlanControllerAssignedDataUpdate ( CFlightPlan FlightPlan,
                                                                       int DataType )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the flight plan reference whose controller
    //                        assigned data is updated
    //      =>  DataType    - the type of the data updated (CTR_DATA_TYPE ...)
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a flight plan controller assigned data is updated.
    //-----------------------------------------------------------------


    //---OnFlightPlanFlightStripPushed---------------------------------

    inline  virtual void    OnFlightPlanFlightStripPushed ( CFlightPlan FlightPlan,
                                                            const char * sSenderController,
                                                            const char * sTargetController )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan          - the flight plan reference whose flight
    //                                stirp is pushed from one controller
    //                                to another
    //      =>  sSenderController   - the one who sent the strip
    //      =>  sTargetController   - the one who received the strip
    //
    //  Description :
    //      This function is called when a strip is pushed to you or
    //      by you. One of SenderController or TargetController is always
    //      you.
    //------------------------------------------------------------------


    //---OnCompileCommand-----------------------------------------------

    inline  virtual bool    OnCompileCommand ( const char * sCommandLine )
    {
        return false ;
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  sCommandLine    - the full command line
    //
    //  Return :
    //      true    - if the command is compiled
    //      false   - else
    //
    //  Description :
    //      When a command is not interpreted by EuroScope it calls the
    //      plugins. Firt this functions are called from the radar
    //      screen handlers. If all refuses then the plug-in level
    //      functions are called too. If one function return true, no
    //      more functions are called.
    //------------------------------------------------------------------


    //---OnOverScreenObject---------------------------------------------

    inline  virtual void    OnOverScreenObject ( int ObjectType,
                                                 const char * sObjectId,
                                                 POINT Pt,
                                                 RECT Area )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  ObjectType  - the type of the screen object
    //      =>  sObjectId   - the ID of the screen object
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object
    //
    //  Description :
    //      This function is called whenever the mouse is over a view
    //      object that was registered by the plugin.
    //------------------------------------------------------------------


    //---OnButtonDownScreenObject---------------------------------------

    inline  virtual void    OnButtonDownScreenObject ( int ObjectType,
                                                       const char * sObjectId,
                                                       POINT Pt,
                                                       RECT Area,
                                                       int Button )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  ObjectType  - the type of the screen object
    //      =>  sObjectId   - the ID of the screen object
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object
    //      =>  Button      - BUTTON_LEFT, BUTTON_MIDDLE, BUTTON_RIGHT
    //
    //  Description :
    //      This function is called whenever the mouse button is pressed
    //      an object that was registered by the plugin.
    //------------------------------------------------------------------


    //---OnButtonUpScreenObject-----------------------------------------

    inline  virtual void    OnButtonUpScreenObject ( int ObjectType,
                                                     const char * sObjectId,
                                                     POINT Pt,
                                                     RECT Area,
                                                     int Button )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  ObjectType  - the type of the screen object
    //      =>  sObjectId   - the ID of the screen object
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object
    //      =>  Button      - BUTTON_LEFT, BUTTON_MIDDLE, BUTTON_RIGHT
    //
    //  Description :
    //      This function is called whenever the mouse button is released
    //      an object that was registered by the plugin.
    //------------------------------------------------------------------


    //---OnClickScreenObject--------------------------------------------

    inline  virtual void    OnClickScreenObject ( int ObjectType,
                                                  const char * sObjectId,
                                                  POINT Pt,
                                                  RECT Area,
                                                  int Button )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  ObjectType  - the type of the screen object
    //      =>  sObjectId   - the ID of the screen object
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object
    //      =>  Button      - BUTTON_LEFT, BUTTON_MIDDLE, BUTTON_RIGHT
    //
    //  Description :
    //      This function is called whenever the user clicks on an object
    //      that was registered by the plugin.
    //------------------------------------------------------------------


    //---OnDoubleClickScreenObject--------------------------------------

    inline  virtual void    OnDoubleClickScreenObject ( int ObjectType,
                                                        const char * sObjectId,
                                                        POINT Pt,
                                                        RECT Area,
                                                        int Button )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  ObjectType  - the type of the screen object
    //      =>  sObjectId   - the ID of the screen object
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object
    //      =>  Button      - BUTTON_LEFT, BUTTON_MIDDLE, BUTTON_RIGHT
    //
    //  Description :
    //      This function is called whenever the user doubleclicks on an
    //      object that was registered by the plugin.
    //------------------------------------------------------------------


    //---OnMoveScreenObject---------------------------------------------

    inline  virtual void    OnMoveScreenObject ( int ObjectType,
                                                 const char * sObjectId,
                                                 POINT Pt,
                                                 RECT Area,
                                                 bool Released )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  ObjectType  - the type of the screen object
    //      =>  sObjectId   - the ID of the screen object
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object
    //      =>  Released    - indicates that this is the last call of the
    //                        move as the user has just released the
    //                        object
    //
    //  Description :
    //      This function is called whenever the user moves the view object.
    //      While it is being dragged this function is called several times
    //      with Released == false. At the end when the object is released
    //      the Released == true, and no more call for the object.
    //------------------------------------------------------------------


    //---OnFunctionCall-------------------------------------------------

    inline  virtual void    OnFunctionCall ( int FunctionId,
                                             const char * sItemString,
                                             POINT Pt,
                                             RECT Area )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  FunctionId  - the ID of the function that is selected
    //      =>  sItemString - the string of the item initiated the
    //                        function call (on popup edit calls it
    //                        holds the updated string)
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object that initiated the function
    //
    //  Description :
    //      This function is called when a function is initiated. The
    //      function itself can be a TAG item function and the user
    //      clicked on a TAG or on an AC list. This function is also
    //      clalled after popup edit and menu items.
    //------------------------------------------------------------------


} ;


class DllSpecEuroScope CFlightPlanList
{
private:

    ESINDEX     m_Position ;// the currently referenced AC list position

    friend  class   CPlugInData ;
    friend  class   CPlugIn ;

public:

    //---CFlightPlanList-----------------------------------------------

    inline  CFlightPlanList ( void )
    {
        m_Position = NULL ;
    } ;


    //---IsValid-------------------------------------------------------

    inline  bool    IsValid ( void ) const
    {
        return m_Position != NULL ;
    } ;


    //---GetColumnNumber-----------------------------------------------

    int     GetColumnNumber ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The number of the columns defined so far. It is a good idea
    //      to test with this function if the AC list is filled from
    //      the settings before adding new columns.
    //-----------------------------------------------------------------


    //---DeleteAllColumns----------------------------------------------

    void    DeleteAllColumns ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It deletes all previous column definitions.
    //-----------------------------------------------------------------


    //---AddColumnDefinition-------------------------------------------

    void    AddColumnDefinition ( const char * sColumnTitle,
                                  int Width,
                                  bool Centered,
                                  const char * sItemProvifer,
                                  int ItemCode,
                                  const char * sLeftButtonFunctionProvifer,
                                  int LeftButtonFunction,
                                  const char * sRightButtonFunctionProvifer,
                                  int RightButtonFunction ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sColumnTitle        - the title string of the column
    //      =>  Width               - width of the column (in chars)
    //      =>  Centered            - indicates centered alignment
    //      =>  sItemProvifer       - TAG item provider for the column; pass
    //                                NULL or "" for ES built in items
    //      =>  ItemCode            - the TAG item codes )see above the
    //                                built in codes)
    //      =>  sLeftButtonFunctionProvifer
    //                              - the provider of the left button function
    //                                (pass NULL, not yet implemented)
    //      =>  LeftButtonFunction  - the left button function (see above
    //                                the built in functions)
    //      =>  sRightButtonFunctionProvifer,
    //                              - the provider of the right button function
    //                                (pass NULL, not yet implemented)
    //      =>  RightButtonFunction - the right button function (see above
    //                                the built in functions)
    //
    //  Description :
    //      It adds one column definition to the AC list. Call this
    //      function after registering the AC list immediately. All
    //      column definitions later will be ignored. Also if there are
    //      data about the content of the list in the settings file then
    //      all calls are ignored.
    //-----------------------------------------------------------------


    //---AddFpToTheList------------------------------------------------

    void    AddFpToTheList ( CFlightPlan FlightPlan ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the FP to be added to the list
    //
    //  Description :
    //      It adds a new FP to the FP list.
    //-----------------------------------------------------------------


    //---RemoveFpFromTheList-------------------------------------------

    void    RemoveFpFromTheList ( CFlightPlan FlightPlan ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the FP to be removed from the list
    //
    //  Description :
    //      It removes a FP to the FP list.
    //-----------------------------------------------------------------


    //---ShowFpList----------------------------------------------------

    void    ShowFpList ( bool Show ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Show    - show/hide
    //
    //  Description :
    //      It shows or hides the FP list.
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CSectorElement
{
private:

    int         m_Position ;// the currently referenced element position
    int         m_ElementType ;               // the type of the element

    friend  class   CPlugInData ;
    friend  class   CPlugIn ;
    friend  class   CRadarScreen ;

public:

    //---CSectorElement------------------------------------------------

    inline  CSectorElement ( void )
    {
        m_Position    = -1 ;
        m_ElementType = 0 ;
    } ;


    //---IsValid-------------------------------------------------------

    inline  bool    IsValid ( void ) const
    {
        return m_Position != -1 ;
    } ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the CTR reference is valid
    //      false   - else
    //
    //  Description :
    //      It tests if the CTR reference is valid.
    //-----------------------------------------------------------------


    //---GetElementType------------------------------------------------

    inline  int     GetElementType ( void ) const
    {
        return m_ElementType ;
    } ;


    //---GetName-------------------------------------------------------

    const char *    GetName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      the name of the element
    //-----------------------------------------------------------------


    //---GetPosition---------------------------------------------------

    bool    GetPosition ( CPosition * pPosition,
                          int Index ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      <=  pPosition   - the position of the element
    //      =>  Index       - the index of the required position (star
    //                        from zero always)
    //
    //  Return :
    //      true    - if the Index if valid within the element and the
    //                position is optained
    //      false   - esle
    //
    //  Remarks :
    //      Not all element have coordinates. This function may return
    //      false for Index zero.
    //-----------------------------------------------------------------


    //---GetComponentName----------------------------------------------

    const char *    GetComponentName ( int Index ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index       - the index of the required component (star
    //                        from zero always)
    //
    //  Return :
    //      the name of the component or
    //      empty string if the index is invalid
    //
    //  Description :
    //      It returns the name of the individual components that can be
    //      switched on or off in the drawings.
    //-----------------------------------------------------------------


    //---GetFrequency--------------------------------------------------

    double  GetFrequency ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      the frequency of the VOR, NDB, AIRPORT elements
    //      0.0 for all the rest
    //-----------------------------------------------------------------


    //---GetRunwayName-------------------------------------------------

    const char *    GetRunwayName ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index       - 0 or 1
    //
    //  Return :
    //      the name of the runway
    //      empty string if the index is invalid
    //
    //  Description :
    //      It returns the name of the runway. Index 0 and 1 is valid
    //      for runway elements while sid/star elements always return
    //      its runway.
    //-----------------------------------------------------------------


    //---GetRunwayHeading----------------------------------------------

    int     GetRunwayHeading ( int Index ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Index       - 0 or 1
    //
    //  Return :
    //      the heading of the runway
    //      -1 if the index is invalid
    //
    //  Description :
    //      It returns the heding name of the runway.
    //-----------------------------------------------------------------


    //---GetAirportName------------------------------------------------

    const char *    GetAirportName ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      the name of the airport this element belongs to
    //      empty string if the type is invalid
    //
    //  Description :
    //      It returns the name of the runway for runway and sid/star
    //      elements.
    //-----------------------------------------------------------------


    //--IsElementActive------------------------------------------------

    bool    IsElementActive ( bool Departure,
                              int Index = 0 ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Departure   - indicates if the elements shoudl be tested
    //                        if active for arrival (false) or
    //                        departure (true)
    //      =>  Index       - 0 or 1
    //
    //  Return :
    //      true    - if the element is active
    //      false   - else
    //
    //  Description :
    //      It returns if the element (that can be airport or runway) is
    //      active for arrival or departure. The Index is used for
    //      runways only.
    //-----------------------------------------------------------------
} ;

class DllSpecEuroScope CGrountToAirChannel
{
private:

    int     m_Index ;         // the currently referenced element index

    friend  class   CPlugIn ;
    friend  class   CPlugInData ;

public:

    inline  CGrountToAirChannel ( void )
    {
        m_Index = -1 ;
    }

    //---IsValid-------------------------------------------------------

    inline  bool    IsValid ( void ) const
    {
        return m_Index != -1 ;
    } ;
    //-----------------------------------------------------------------
    //  Return :
    //      true    - if the CTR reference is valid
    //      false   - else
    //
    //  Description :
    //      It tests if the CTR reference is valid.
    //-----------------------------------------------------------------


    //---GetName-------------------------------------------------------

    const char * GetName ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The name of the communication channel.
    //-----------------------------------------------------------------


    //---GetFrequency--------------------------------------------------

    double GetFrequency ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The frequency of the communication channel.
    //-----------------------------------------------------------------


    //---GetVoiceServer------------------------------------------------

    const char * GetVoiceServer ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The voice server name of the communication channel.
    //-----------------------------------------------------------------


    //---GetVoiceChannel-----------------------------------------------

    const char * GetVoiceChannel ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The voice server channel name of the communication channel.
    //-----------------------------------------------------------------


    //---GetIsPrimary--------------------------------------------------

    bool GetIsPrimary ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the channel is the primary channel.
    //-----------------------------------------------------------------


    //---GetIsAtis-----------------------------------------------------

    bool GetIsAtis ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the channel is the ATIS channel.
    //-----------------------------------------------------------------


    //---GetIsTextReceiveOn--------------------------------------------

    bool GetIsTextReceiveOn ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the channel is receiving text messages.
    //-----------------------------------------------------------------


    //---GetIsTextTransmitOn-------------------------------------------

    bool GetIsTextTransmitOn ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the channel is transmitting text messages.
    //-----------------------------------------------------------------


    //---GetIsVoiceReceiveOn-------------------------------------------

    bool GetIsVoiceReceiveOn ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the channel is receiving voice. This
    //      flag is set when the check box is set. It alone does not
    //      mean the server connection is on.
    //-----------------------------------------------------------------


    //---GetIsVoiceTransmitOn------------------------------------------

    bool GetIsVoiceTransmitOn ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the channel is transmitting voice.
    //-----------------------------------------------------------------


    //---GetIsVoiceConnected-------------------------------------------

    bool GetIsVoiceConnected ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flag indicating if the channel is successfully connected
    //      to the voice server.
    //-----------------------------------------------------------------


    //---TogglePrimary-------------------------------------------------

    void    TogglePrimary ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It changes the primary setting of the channel (like clicking
    //      on the check box).
    //-----------------------------------------------------------------

    
    //---ToggleAtis----------------------------------------------------

    void    ToggleAtis ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It changes the ATIS setting of the channel (like clicking
    //      on the check box).
    //-----------------------------------------------------------------


    //---ToggleTextReceive---------------------------------------------

    void    ToggleTextReceive ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It changes the text receive setting of the channel (like clicking
    //      on the check box).
    //-----------------------------------------------------------------


    //---ToggleTextTransmit--------------------------------------------

    void    ToggleTextTransmit ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It changes the text transmit setting of the channel (like clicking
    //      on the check box).
    //-----------------------------------------------------------------


    //---ToggleVoiceReceive--------------------------------------------

    void    ToggleVoiceReceive ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It changes the voice receive setting of the channel (like clicking
    //      on the check box).
    //-----------------------------------------------------------------


    //---ToggleVoiceTransmit-------------------------------------------

    void    ToggleVoiceTransmit ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It changes the voice transmit setting of the channel (like clicking
    //      on the check box).
    //-----------------------------------------------------------------

} ;


class DllSpecEuroScope CPlugIn
{
private:

    CPlugInData * m_pPluginData ;
                              // pointer to the internal data structure

public:

    //---CPlugIn-------------------------------------------------------

    CPlugIn ( int CompatibilityCode,
              const char * sPlugInName,
              const char * sVersionNumber,
              const char * sAuthorName,
              const char * sCopyrigthMessage ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CompatibilityCode   - for compatiblity check
    //      =>  sPlugInName         - the user readable name of the plugin
    //      =>  sVersionNumber      - actual version number
    //      =>  sAuthorName         - the name of the author
    //      =>  sCopyrightMessage   - the name of the author
    //
    //  Description :
    //      The default constructor.
    //-----------------------------------------------------------------


    //---~CPlugIn------------------------------------------------------

    virtual ~CPlugIn ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      The descriptoz. Keep it virtual.
    //-----------------------------------------------------------------


    //---OnControllerPositionUpdate------------------------------------

    inline  virtual void    OnControllerPositionUpdate ( CController Controller )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Controller  - the controller reference whose position
    //                        is updated
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a controller position is updated.
    //-----------------------------------------------------------------


    //---OnControllerDisconnect----------------------------------------

    inline  virtual void    OnControllerDisconnect ( CController Controller )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Controller  - the controller reference
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a controller is logged off or its position
    //      update is timed out.
    //-----------------------------------------------------------------


    //---OnRadarTergetPositionUpdate--------------------------------------

    inline  virtual void    OnRadarTargetPositionUpdate ( CRadarTarget RadarTarget )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  radarTarget - the radar target reference whose position
    //                        is updated
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a radara target position is updated.
    //-----------------------------------------------------------------


    //---OnFlightPlanDisconnect----------------------------------------

    inline  virtual void    OnFlightPlanDisconnect ( CFlightPlan FlightPlan )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the flight plan reference
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a fight plan is logged off or its position
    //      update is timed out.
    //-----------------------------------------------------------------


    //---OnFlightPlanFlightPlanDataUpdate------------------------------

    inline  virtual void    OnFlightPlanFlightPlanDataUpdate ( CFlightPlan FlightPlan )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the flight plan reference whose flight plan
    //                        data is updated
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a flight plan flight plan data is updated.
    //-----------------------------------------------------------------


    //---OnPlaneInformationUpdate--------------------------------------

    inline  virtual void    OnPlaneInformationUpdate ( const char * sCallsign,
                                                       const char * sLivery,
                                                       const char * sPlaneType )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sCallsign   - the callsign of the plane
    //      =>  sLivery     - the livery set in the pilot client
    //      =>  sPlaneType  - the manufacturer and type set  in the pilot client
    //
    //  Description :
    //      This function is called when a SqwakBox plane information
    //      update message is received.
    //-----------------------------------------------------------------


    //---OnFlightPlanControllerAssignedDataUpdate----------------------

    inline  virtual void    OnFlightPlanControllerAssignedDataUpdate ( CFlightPlan FlightPlan,
                                                                       int DataType )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the flight plan reference whose controller
    //                        assigned data is updated
    //      =>  DataType    - the type of the data updated (CTR_DATA_TYPE ...)
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a plane controller assigned data is updated.
    //-----------------------------------------------------------------


    //---OnFlightPlanFlightStripPushed---------------------------------

    inline  virtual void    OnFlightPlanFlightStripPushed ( CFlightPlan FlightPlan,
                                                            const char * sSenderController,
                                                            const char * sTargetController )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan          - the FP reference whose flight
    //                                stirp is pushed from one controller
    //                                to another
    //      =>  sSenderController   - the one who sent the strip
    //      =>  sTargetController   - the one who received the strip
    //
    //  Description :
    //      This function is called when a strip is pushed to you or
    //      by you. One of SenderController or TargetController is always
    //      you.
    //------------------------------------------------------------------


    //---OnRadarScreenCreated------------------------------------------

    inline  virtual CRadarScreen    * OnRadarScreenCreated ( const char * sDisplayName,
                                                             bool NeedRadarContent,
                                                             bool GeoReferenced,
                                                             bool CanBeSaved,
                                                             bool CanBeCreated )
    {
        return NULL ;
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sDisplayName    - the name of the display that is just
    //                            created; if the name is 
    //                            "Standard ES radar screen"
    //                            it is a standard ES radar screen
    //      =>  NeedRadarContent,
    //      =>  GeoReferenced,
    //      =>  CanBeSaved,
    //      =>  CanBeCreated    - other radar screen attributes
    //                            see them at RegisterDisplayType
    //
    //  Return :
    //      an instance to your own radar scren class member or
    //      NULL if this type is not handled by your plug-in
    //
    //  Description :
    //      When a new radar display is opened your plug-in is called
    //      to create an screen object instance. You may create your own
    //      radar handling class that is derived from CRadarScreen.
    //      Allocate a new instance and return the pointer to handle
    //      all the events from there.
    //-----------------------------------------------------------------


    //---OnCompileCommand-----------------------------------------------

    inline  virtual bool    OnCompileCommand ( const char * sCommandLine )
    {
        return false ;
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  sCommandLine    - the full command line
    //
    //  Return :
    //      true    - if the command is compiled
    //      false   - else
    //
    //  Description :
    //      When a command is not interpreted by EuroScope it calls the
    //      plugins. Firt the radar screens are called. If all refuses
    //      then the plug-in level functions are called too. If one
    //      function return true, no more functions are called.
    //------------------------------------------------------------------


    //---OnCompileFrequencyChat-----------------------------------------

    inline  virtual void    OnCompileFrequencyChat ( const char * sSenderCallsign,
                                                     double Frequency,
                                                     const char * sChatMessage )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  sSenderCallsign - the sender
    //      =>  Frequency       - the frequency where you received the
    //                            message
    //      =>  sChatMessage    - the full message string
    //
    //  Description :
    //      When a frequency message is received you have the possibility
    //      to process, display its content. This function may be called
    //      for several times with the same message if it is sent to
    //      several frequencies.
    //------------------------------------------------------------------


    //---OnCompilePrivateChat-------------------------------------------

    inline  virtual void    OnCompilePrivateChat ( const char * sSenderCallsign,
                                                   const char * sReceiverCallsign,
                                                   const char * sChatMessage )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  sSenderCallsign - the sender
    //      =>  sReceiverCallsign - the receivers callsign
    //                              server   - for messages from the SERVER
    //                              ATC      - ATC messages
    //                              Broadcast- broadcast messages
    //                              SUP      - supervisor requests
    //      =>  sChatMessage    - the full message string
    //
    //  Description :
    //      When a private message is received you have the possibility
    //      to process, display its content.
    //------------------------------------------------------------------


    //---OnGetTagItem---------------------------------------------------

    inline  virtual void    OnGetTagItem ( CFlightPlan FlightPlan,
                                           CRadarTarget RadarTarget,
                                           int ItemCode,
                                           int TagData,
                                           char sItemString [ 16 ],
                                           int * pColorCode,
                                           COLORREF * pRGB,
                                           double * pFontSize )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  FlightPlan  - the FP reference whose TAG item is
    //                        to be created - it may be invalid
    //      =>  RadarTraget - the RT reference whose TAG item is
    //                        to be created - it may be invalid
    //      =>  ItemCode    - the code of the item
    //      =>  TagData     - the data available for the TAG; values are
    //                          - TAG_DATA_UNCORRELATED_RADAR
    //                          - TAG_DATA_FLIGHT_PLAN_TRACK
    //                          - TAG_DATA_CORRELATED
    //      <=  sItemString - the generated string; it is strictly limited
    //                        to 15 chars (plust terminator \0)
    //      <=  pColorCode  - the color code of the item; return one of
    //                        the TAG_COLOR_ ... value
    //                        TAG_COLOR_DEFAULT - indicates the default for AC state
    //                        TAG_COLOR_RGB_DEFINED - do not use and
    //                        predefined color, but the return value
    //                        in pRGB
    //      <=  pRGB        - the requested color of the TAG item; it is
    //                        used only in case if pColorCode is
    //                        TAG_COLOR_RGB_DEFINED
    //      <=> pFontSize   - the requested size of the font
    //
    //  Description :
    //      All plugins must implement this function to handle the
    //      events when a plane position is updated.
    //-----------------------------------------------------------------


    //---OnRefreshFpListContent----------------------------------------

    inline  virtual void    OnRefreshFpListContent ( CFlightPlanList AcList )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  AcList  - the AC list to be refreshed
    //
    //  Description :
    //      This function is called once in a second just after all plane
    //      data is refreshed in ES memory. You may add/remove planes
    //      to/from the list, or clear the content and add the planes
    //      again.
    //-----------------------------------------------------------------


    //---OnNewMetarReceived--------------------------------------------

    inline  virtual void    OnNewMetarReceived ( const char * sStation,
                                                 const char * sFullMetar )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sStation    - the airport
    //      =>  sFullMetar  - the original comlete metar string
    //
    //  Description :
    //      This function is called whenever a new metar data is received
    //      from the server.
    //-----------------------------------------------------------------


    //---OnFunctionCall-------------------------------------------------

    inline  virtual void    OnFunctionCall ( int FunctionId,
                                             const char * sItemString,
                                             POINT Pt,
                                             RECT Area )
    {
    } ;
    //------------------------------------------------------------------
    //  Parameters :
    //      =>  FunctionId  - the ID of the function that is selected
    //      =>  sItemString - the string of the item initiated the
    //                        function call (on popup edit calls it
    //                        holds the updated string)
    //      =>  Pt          - the mouse position
    //      =>  Area        - the area originally registered for the
    //                        screen object that initiated the function
    //
    //  Description :
    //      This function is called when a function is initiated. The
    //      function itself can be a TAG item function and the user
    //      clicked on a TAG or on an AC list. This function is also
    //      clalled after popup edit and menu items.
    //------------------------------------------------------------------


    //---OnAirportRunwayActivityChanged---------------------------------

    inline  virtual void    OnAirportRunwayActivityChanged ( void )
    {
    } ;
    //-----------------------------------------------------------------
    //  Description :
    //      This function is called whenever the user closes the
    //      runway activity dialog with OK.
    //-----------------------------------------------------------------


    //---OnAirportRunwayActivityChanged---------------------------------

    inline  virtual void    OnTimer ( int Counter )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Counter - It counts the secods from the application start
    //      
    //  Description :
    //      This function is called once in a second. Timer is a limited
    //      resource and you can use the one that refreshes the sceens
    //      once in a second.
    //-----------------------------------------------------------------


    //---GetPlugInName-------------------------------------------------

    const char *    GetPlugInName ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      the plugin name.
    //-----------------------------------------------------------------


    //---RegisterDisplayType-------------------------------------------

    void    RegisterDisplayType ( const char * sDisplayName,
                                  bool NeedRadarContent,
                                  bool GeoReferenced,
                                  bool CanBeSaved,
                                  bool CanBeCreated ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sDisplayType    - the name of the display type
    //      =>  NeedRadarContent- indicates that the default radar content
    //                            should be displayed in this type
    //      =>  GeoReferenced   - indicates that lat/lon coordnates
    //                            can be used in the type, and therefore
    //                            zooming in/out a panning is allowed
    //      =>  CanBeSaved      - indicates that ASR file content can
    //                            be saved on exit
    //      =>  CanBeCreated    - indicates that it should be listed in
    //                            new ASR types
    //
    //  Description :
    //      Call this function to register your own display type within
    //      EuroScope. You need this only in case you would like to
    //      create a complete new outllok of the view. In your own type
    //      you are free to display anything to the screen.
    //-----------------------------------------------------------------


    //---RegisterTagItemType-------------------------------------------

    void    RegisterTagItemType ( const char * sDisplayName,
                                  int Code ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sDisplayName    - the user readable name of the TAG item
    //      =>  Code            - the item code (must be bigger than 0)
    //
    //  Description :
    //      It registers a new TAG item type.
    //-----------------------------------------------------------------


    //---RegisterTagItemFunction---------------------------------------

    void    RegisterTagItemFunction ( const char * sDisplayName,
                                      int Code ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sDisplayName    - the user readable name of the TAG item
    //      =>  Code            - the item code (must be bigger than 0)
    //
    //  Description :
    //      It registers a new TAG item function.
    //-----------------------------------------------------------------


    //---RegisterFpList------------------------------------------------

    CFlightPlanList RegisterFpList ( const char * sListName ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sListName   - the name of the list box
    //
    //  Return :
    //      The new AC list instance. The returned instance may be saved
    //      into your memory area and can be used in further processing.
    //
    //  Description :
    //      It registers a new AC list.
    //-----------------------------------------------------------------


    //---RegisterToolbarItem-------------------------------------------

    void    RegisterToolbarItem ( int ItemId,
                                  const char * sItemName ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  ItemId      - the ID of the toolbar item
    //      =>  sItemName   - the name of the toolbar item
    //
    //  Decsription :
    //      It registers a toolbar item for the plugin. To display it
    //      implement the OnToolbarItem... functions.
    //-----------------------------------------------------------------


    //---RefreshToolbar------------------------------------------------

    void    RefreshToolbar ( bool ResizeToo ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  ResizeToo   - indicates that the plugin would liek to
    //                        change the size of the toolbar item
    //
    //  Description :
    //      Call this function to indicate that the toolbar content is
    //      invalid.
    //-----------------------------------------------------------------


    //---SaveDataToSettings--------------------------------------------

    void    SaveDataToSettings ( const char * sVariableName,
                                 const char * sVariableDescription,
                                 const char * sValue ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sVariableName       - the name under your value will be saved
    //      =>  sVariableDescription- a user readable name of the variable;
    //                                this value will be shown in the
    //                                save modified settings dialog; if
    //                                empty (never pass NULL), then it
    //                                will not be shown
    //      =>  sValue              - the value itself to be stored; it
    //                                may contain only printable chars and
    //                                never LF or CR or ':'.
    //
    //  Description :
    //      It saves the value for the plug in the settings file.
    //-----------------------------------------------------------------


    //---GetDataFromSettings-------------------------------------------

    const char  * GetDataFromSettings ( const char * sVariableName ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sVariableName       - the name under your value was saved
    //
    //  Return :
    //      the value found in the ASR or
    //      NULL if not found
    //
    //  Description :
    //      It loads the value for the plug in from the settings file.
    //-----------------------------------------------------------------


    //---OpenPopupEdit-------------------------------------------------

    void    OpenPopupEdit ( RECT Area,
                            int FunctionId,
                            const char * sInitialValue ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Area            - the rectangle where the edit box
    //                            appears (it is a good idea to pass the
    //                            rectangle coming from the OnFunctionCall)
    //      =>  FunctionId      - the ID of the editing mode; this value
    //                            will be passed to the OnFunctionCall
    //      =>  sInitialValue   - the initial content of the edit box
    //
    //  Description :
    //      It opens up a popup edit box in the Area rectangle with the
    //      sInitialContent. After editing it by the user the
    //      OnFunctionCall is called with the ID and with the new
    //      string value.
    //-----------------------------------------------------------------


    //---OpenPopupList-------------------------------------------------

    void    OpenPopupList ( RECT Area,
                            const char * sTitle,
                            int ColumnNumber ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Area        - the requested position of the center
    //                        (selected element) in the list
    //      =>  sTitle      - the popup list title
    //      =>  ColumnNumber- the number of the columns in the list
    //                        (currently it is limited to 1 and 2)
    //
    //  Description :
    //      It starts opening a popup list. You must call the
    //      AddPopupListElement to fill the list with content. Without
    //      that it will not be displayed. The popup list is a global
    //      resource of EuroScope and there is only one list available.
    //-----------------------------------------------------------------


    //---AddPopupListElement-------------------------------------------

    void    AddPopupListElement ( const char * sString1,
                                  const char * sString2,
                                  int FunctionId,
                                  bool Selected = false,
                                  int Checked = POPUP_ELEMENT_NO_CHECKBOX,
                                  bool Disabled = false,
                                  bool Fixed = false ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sString1,
    //          sString2    - the strings to be displayed in the first
    //                        and in the second column (in case of two
    //                        columns); sString1 will be passed as
    //                        string parameter to OnFunctionCall if the
    //                        item is selected
    //      =>  FunctionId  - the function ID to be passed to
    //                        OnFunctionCall if the element is selected
    //      =>  Selected    - indicates if the element is the actual
    //                        value (pass true only once in a definition)
    //      =>  Checked     - indicates if the item needs a check box in
    //                        from or not; and if it has check box then
    //                        checked or unchecked
    //      =>  Disabled    - indicates if the element can not be selected
    //      =>  Fixed       - fixed elements are the last ones; they
    //                        appear in the bottom and are not scrolled
    //                        with the rest; never add more than 3-4 fixed
    //                        items
    //
    //  Description :
    //      It adds a new popup element to the popup list.
    //-----------------------------------------------------------------


    //---GetConnectionType---------------------------------------------

    int     GetConnectionType ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      the actual connection type - on of CONNECTION_TYPE_...
    //-----------------------------------------------------------------


    //---SelectActiveSectorfile----------------------------------------

    void    SelectActiveSectorfile ( void ) ;
    //-----------------------------------------------------------------
    //  Description :
    //      It selects the active sectorfile as the source for the
    //      subsequent SectorElementFirst and SectorElementNext calls.
    //-----------------------------------------------------------------


    //---SelectScreenSectorfile----------------------------------------

    void    SelectScreenSectorfile ( CRadarScreen * pRadarScreen ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  pRadarScreen    - the screen reference
    //
    //  Description :
    //      It selects the active sectorfile as the source for the
    //      subsequent SectorElementFirst and SectorElementNext calls.
    //-----------------------------------------------------------------


    //---SetASELAircraft-----------------------------------------------

    void    SetASELAircraft ( const CFlightPlan FlightPlan ) ;
    void    SetASELAircraft ( const CRadarTarget RadarTarget ) ;
    //-----------------------------------------------------------------
    //  Parameter :
    //      =>  FlightPlan  - the FP to be selected as ASEL
    //      =>  RadarTarget - the RT to be selected as ASEL
    //
    //  Description :
    //      It sets the given FP/RT as ASEL.
    //-----------------------------------------------------------------


    //---FlightPlanSelect----------------------------------------------

    CFlightPlan FlightPlanSelect ( const char * sCallsign ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sCallsign   - the callsign of the FP to be selected
    //
    //  Return :
    //      A flight plan object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the FP with the given callsign. 
    //-----------------------------------------------------------------


    //---RadarTargetSelect---------------------------------------------

    CRadarTarget    RadarTargetSelect ( const char * sCallsign ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sCallsign   - the callsign of the RT to be selected
    //                        you can also enter system ID as callsign
    //
    //  Return :
    //      A radar target object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the RT with the given callsign/system ID. 
    //-----------------------------------------------------------------


    //---FlightPlanSelectFirst-----------------------------------------

    CFlightPlan FlightPlanSelectFirst ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      An flight plan object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the first FP in the list. 
    //-----------------------------------------------------------------


    //---RadarTargetSelectFirst----------------------------------------

    CRadarTarget    RadarTargetSelectFirst ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      An radar target object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the first RT in the list. 
    //-----------------------------------------------------------------


    //---FlightPlanSelectNext------------------------------------------

    CFlightPlan FlightPlanSelectNext ( CFlightPlan CurrentFlightPlan ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CurrentFlightPlan   - the actually selected FP element
    //
    //  Return :
    //      A flight plan object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the next FP in the list.
    //-----------------------------------------------------------------


    //---RadarTargetSelectNext-----------------------------------------

    CRadarTarget    RadarTargetSelectNext ( CRadarTarget CurrentRadartarget ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CurrentRadartarget  - the actually selected RT element
    //
    //  Return :
    //      A radar target object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the next RT in the list.
    //-----------------------------------------------------------------


    //---FlightPlanSelectASEL------------------------------------------

    CFlightPlan FlightPlanSelectASEL ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A flight plan object instance. May be invalid.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the currently ASEL flight plan.
    //-----------------------------------------------------------------


    //---RadarTargetSelectASEL-----------------------------------------

    CRadarTarget    RadarTargetSelectASEL ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A radar target object instance. May be invalid.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the currently ASEL radar target.
    //-----------------------------------------------------------------


    //---ControllerSelect----------------------------------------------

    CController ControllerSelect ( const char * sCallsign ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sCallsign   - the callsign of the controller to be selected
    //
    //  Return :
    //      A controller object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the controller with the given callsign. 
    //-----------------------------------------------------------------


    //---ControllerSelectByPositionId----------------------------------

    CController ControllerSelectByPositionId ( const char * sPositionId ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sPositionId - the position ID of the controller to be selected
    //
    //  Return :
    //      A controller object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the controller with the given position ID. 
    //-----------------------------------------------------------------


    //---ControllerMyself----------------------------------------------

    CController ControllerMyself ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A controller object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the controller that was logged in in this session. 
    //-----------------------------------------------------------------


    //---ControllerSelectFirst-----------------------------------------

    CController ControllerSelectFirst ( void ) const ;
    //-----------------------------------------------------------------
    //  Return :
    //      A controller object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the first controller in the list. 
    //-----------------------------------------------------------------


    //---ControllerSelectNext------------------------------------------

    CController ControllerSelectNext ( CController CurrentController ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CurrentController   - the actually selected controller element
    //
    //  Return :
    //      A controller object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the next controller in the list.
    //-----------------------------------------------------------------


    //---SectorFileElementSelectFirst----------------------------------

    CSectorElement  SectorFileElementSelectFirst ( int ElementType ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  ElementType - the type of the elements to be searched
    //                        for; pass SECTOR_ELEMENT_ALL to search
    //                        for all element type
    //
    //  Return :
    //      A sector file element object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the first element (of the given type) from the
    //      previously selected sectorfile. 
    //-----------------------------------------------------------------


    //---SectorFileElementSelectNext-----------------------------------

    CSectorElement  SectorFileElementSelectNext ( CSectorElement CurrentElement,
                                                  int ElementType ) const ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CurrentElement  - the actually selected sectorfile element
    //      =>  ElementType     - the type of the elements to be searched
    //                            for; pass SECTOR_ELEMENT_ALL to search
    //                            for all element type
    //
    //  Return :
    //      A sectrofile element object instance.
    //
    //  Remark:
    //      This instance is only valid inside the block you are querying.
    //      Do not save it to a static place or into a member variables.
    //      Subsequent use of an invalid extracted route reference may
    //      cause ES to crash.
    //
    //  Description :
    //      It selects the next element (of the given type) from the
    //      previously selected sectorfile. 
    //-----------------------------------------------------------------


    //---DisplayUserMessage--------------------------------------------

    void    DisplayUserMessage ( const char * sHandlerName,
                                 const char * sSenderName,
                                 const char * sMessage,
                                 bool ShowHandler,
                                 bool ShowUnread,
                                 bool ShowUnreadEvenIfBusy,
                                 bool StartFlashing,
                                 bool NeedConfirmation ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sHandlerName    - the message handler name; this name
    //                            above the chat area (the callsign in
    //                            case of private chat, ATC, Broadcast,
    //                            etc.); use your plug-in name here
    //      =>  sSenderName     - the name of the sender; if empty the
    //                            message will be displayed if the user
    //                            were sending it
    //      =>  sMessage        - the message itself
    //      =>  ShowHandler     - should the handler be displayed (if hidden)
    //      =>  ShowUnread      - should the handler be colored with
    //                            unread message color if not selected
    //      =>  ShowUnreadEvenIfBusy    - as above even if busy flag is set
    //      =>  StartFlashing   - should the handler be flashing
    //      =>  NeedConfirmation- does this message need a confirmation
    //
    //  Description :
    //      This function writes a message to the user to the standard
    //      chat area.
    //-----------------------------------------------------------------


    //---GroundToArChannelSelectFirst----------------------------------

    CGrountToAirChannel GroundToArChannelSelectFirst ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The first channel defined in the Voice Communication Setup
    //      (not necessarily the first in the dialog). The return value
    //      is temporary. Do not save it for further use.
    //-----------------------------------------------------------------


    //---GroundToArChannelSelectNext-----------------------------------

    CGrountToAirChannel GroundToArChannelSelectNext ( CGrountToAirChannel CurrentChannel ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  CurrentChannel  - the current channel
    //
    //  Return :
    //      The next channel defined in the Voice Communication Setup
    //      (not necessarily the next in the dialog). The return value
    //      is temporary. Do not save it for further use.
    //-----------------------------------------------------------------


    //---AddAlias------------------------------------------------------

    void AddAlias ( const char * sAliasName,
                    const char * sAliasValue ) ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  sAliasName  - the alias name (must begin with .)
    //      =>  sAliasValue - the value to be used
    //
    //  Description :
    //      It adds a new alias to the alias interpreter.
    //-----------------------------------------------------------------


    //---OnVoiceTransmitStarted----------------------------------------

    inline  virtual void    OnVoiceTransmitStarted ( bool OnPrimary )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  OnPrimary   - indicates if the transmit started on the
    //                        primary voice channel
    //
    //  Description :
    //      This function is called on a voice transmit start.
    //      Warning: This function is called from a seperated thread,
    //      write a thread-safe handler.
    //-----------------------------------------------------------------


    //---OnVoiceTransmitEnded------------------------------------------

    inline  virtual void    OnVoiceTransmitEnded ( bool OnPrimary )
    {
    } ;
    //-----------------------------------------------------------------
    //      =>  OnPrimary   - indicates if the transmit ended on the
    //                        primary voice channel
    //
    //  Description :
    //      This function is called on a voice transmit end.
    //      Warning: This function is called from a seperated thread,
    //      write a thread-safe handler.
    //-----------------------------------------------------------------


    //---OnVoiceReceiveStarted-----------------------------------------

    inline  virtual void    OnVoiceReceiveStarted ( CGrountToAirChannel Channel )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Channel - the channel that receives some voice
    //
    //  Description :
    //      This function is called on a voice receive start.
    //-----------------------------------------------------------------


    //---OnVoiceReceiveEnded-------------------------------------------

    inline  virtual void    OnVoiceReceiveEnded ( CGrountToAirChannel Channel )
    {
    } ;
    //-----------------------------------------------------------------
    //  Parameters :
    //      =>  Channel - the channel that received some voice
    //
    //  Description :
    //      This function is called on a voice receive start.
    //-----------------------------------------------------------------


    //---GetTransitionAltitude-----------------------------------------

    int     GetTransitionAltitude ( void ) ;
    //-----------------------------------------------------------------
    //  Return :
    //      The transition altitude of the system.
    //-----------------------------------------------------------------

} ;

} // end of EuroScopePlugIn namespace


//---EuroScopePlugInInit-----------------------------------------------

void    __declspec ( dllexport )    EuroScopePlugInInit ( EuroScopePlugIn :: CPlugIn ** ppPlugInInstance ) ;
//---------------------------------------------------------------------
//  Parameters :
//      <=  ppPlugInInstance- a pointer to the plug-in instance
//
//  Description :
//      Every plugin have to implement this function. When EuroScope
//      loads the DLL, it searches for this function and calls it. Do
//      all initialization code (including the plugin class instance
//      creation) in this function. Only one plug-in instance is
//      supported by in one DLL
//      Before returning you MUST call the SetPlugInName function.
//      If this call is missing or any parameter is empty your DLL will
//      be unloaded immediately.
//---------------------------------------------------------------------


//---EuroScopePlugInExit-----------------------------------------------

void    __declspec ( dllexport )    EuroScopePlugInExit ( void ) ;
//---------------------------------------------------------------------
//  Description :
//      Every plugin have to implement this function. When EuroScope
//      unloads the DLL (for user request or due exiting) it will call
//      this function. Be sure to clean up all members, especialy
//      delete all plugin class instances.
//---------------------------------------------------------------------


