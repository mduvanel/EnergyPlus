// EnergyPlus, Copyright (c) 1996-2023, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// EnergyPlus::AirflowNetwork HVAC-related unit tests

// Google test headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <AirflowNetwork/Elements.hpp>
#include <AirflowNetwork/Solver.hpp>
#include <EnergyPlus/BranchNodeConnections.hh>
#include <EnergyPlus/CurveManager.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataAirLoop.hh>
#include <EnergyPlus/DataAirSystems.hh>
#include <EnergyPlus/DataDefineEquip.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataIPShortCuts.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DataZoneEquipment.hh>
#include <EnergyPlus/Fans.hh>
#include <EnergyPlus/HVACStandAloneERV.hh>
#include <EnergyPlus/HeatBalanceAirManager.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/IOFiles.hh>
#include <EnergyPlus/InternalHeatGains.hh>
#include <EnergyPlus/Material.hh>
#include <EnergyPlus/OutAirNodeManager.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/SimAirServingZones.hh>
#include <EnergyPlus/SimulationManager.hh>
#include <EnergyPlus/SurfaceGeometry.hh>
#include <EnergyPlus/UtilityRoutines.hh>
#include <EnergyPlus/WaterThermalTanks.hh>
#include <EnergyPlus/ZoneAirLoopEquipmentManager.hh>
#include <EnergyPlus/ZoneTempPredictorCorrector.hh>

#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace AirflowNetwork;
using namespace DataSurfaces;
using namespace DataHeatBalance;
using namespace EnergyPlus::DataLoopNode;
using namespace EnergyPlus::ScheduleManager;
using namespace OutAirNodeManager;
using namespace EnergyPlus::Fans;
using namespace EnergyPlus::HVACStandAloneERV;
using namespace EnergyPlus::DataHVACGlobals;

namespace EnergyPlus {

TEST_F(EnergyPlusFixture, AirflowNetwork_TestZoneVentingSch)
{

    // Unit test for #5021

    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "SALA DE AULA";

    state->dataSurface->Surface.allocate(2);
    state->dataSurface->Surface(1).Name = "WINDOW AULA 1";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "SALA DE AULA";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 90.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(2).Name = "WINDOW AULA 2";
    state->dataSurface->Surface(2).Zone = 1;
    state->dataSurface->Surface(2).ZoneName = "SALA DE AULA";
    state->dataSurface->Surface(2).Azimuth = 180.0;
    state->dataSurface->Surface(2).ExtBoundCond = 0;
    state->dataSurface->Surface(2).HeatTransSurf = true;
    state->dataSurface->Surface(2).Tilt = 90.0;
    state->dataSurface->Surface(2).Sides = 4;

    SurfaceGeometry::AllocateSurfaceWindows(*state, 2);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;
    state->dataSurface->SurfWinOriginalClass(2) = DataSurfaces::SurfaceClass::Window;
    state->dataGlobal->NumOfZones = 1;

    std::string const idf_objects = delimited_string({
        "Schedule:Constant,OnSch,,1.0;",
        "Schedule:Constant,Aula people sched,,0.0;",
        "Schedule:Constant,Sempre 21,,21.0;",
        "AirflowNetwork:SimulationControl,",
        "  NaturalVentilation, !- Name",
        "  MultizoneWithoutDistribution, !- AirflowNetwork Control",
        "  SurfaceAverageCalculation, !- Wind Pressure Coefficient Type",
        "  , !- Height Selection for Local Wind Pressure Calculation",
        "  LOWRISE, !- Building Type",
        "  1000, !- Maximum Number of Iterations{ dimensionless }",
        "  LinearInitializationMethod, !- Initialization Type",
        "  0.0001, !- Relative Airflow Convergence Tolerance{ dimensionless }",
        "  0.0001, !- Absolute Airflow Convergence Tolerance{ kg / s }",
        "  -0.5, !- Convergence Acceleration Limit{ dimensionless }",
        "  90, !- Azimuth Angle of Long Axis of Building{ deg }",
        "  0.36;                    !- Ratio of Building Width Along Short Axis to Width Along Long Axis",
        "AirflowNetwork:MultiZone:Zone,",
        "  sala de aula, !- Zone Name",
        "  Temperature, !- Ventilation Control Mode",
        "  Sempre 21, !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "  1, !- Minimum Venting Open Factor{ dimensionless }",
        "  , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "  100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "  , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "  300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "  Aula people sched, !- Venting Availability Schedule Name",
        "  Standard;                !- Single Sided Wind Pressure Coefficient Algorithm",
        "AirflowNetwork:MultiZone:Surface,",
        "  window aula 1, !- Surface Name",
        "  Simple Window, !- Leakage Component Name",
        "  , !- External Node Name",
        "  1, !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "  ZoneLevel, !- Ventilation Control Mode",
        "  , !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "  , !- Minimum Venting Open Factor{ dimensionless }",
        "  , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "  100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "  , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "  300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "  Aula people sched;       !- Venting Availability Schedule Name",
        "AirflowNetwork:MultiZone:Surface,",
        "  window aula 2, !- Surface Name",
        "  Simple Window, !- Leakage Component Name",
        "  , !- External Node Name",
        "  1, !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "  Temperature, !- Ventilation Control Mode",
        "  Sempre 21, !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "  1, !- Minimum Venting Open Factor{ dimensionless }",
        "  , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "  100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "  , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "  300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "  Aula people sched;       !- Venting Availability Schedule Name",
        "AirflowNetwork:MultiZone:Component:SimpleOpening,",
        "  Simple Window, !- Name",
        "  0.0010, !- Air Mass Flow Coefficient When Opening is Closed{ kg / s - m }",
        "  0.65, !- Air Mass Flow Exponent When Opening is Closed{ dimensionless }",
        "  0.01, !- Minimum Density Difference for Two - Way Flow{ kg / m3 }",
        "  0.78;                    !- Discharge Coefficient{ dimensionless }",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    state->afn->get_input();

    // MultizoneZoneData has only 1 element so may be hardcoded
    auto GetIndex = UtilityRoutines::FindItemInList(state->afn->MultizoneZoneData(1).VentingSchName,
                                                    state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules}));
    EXPECT_EQ(GetIndex, state->afn->MultizoneZoneData(1).VentingSchNum);

    state->dataHeatBal->Zone.deallocate();
    state->dataSurface->Surface.deallocate();
    state->dataSurface->SurfaceWindow.deallocate();
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestPressureStat)
{

    // Unit test for a new feature of PressureStat and #5687
    int i;

    std::string const idf_objects = delimited_string({
        "  Building,",
        "    Small Office with AirflowNetwork model,  !- Name",
        "    0,                       !- North Axis {deg}",
        "    Suburbs,                 !- Terrain",
        "    0.001,                   !- Loads Convergence Tolerance Value",
        "    0.0050000,               !- Temperature Convergence Tolerance Value {deltaC}",
        "    FullInteriorAndExterior, !- Solar Distribution",
        "    25,                      !- Maximum Number of Warmup Days",
        "    6;                       !- Minimum Number of Warmup Days",

        "  Timestep,6;",

        "  SurfaceConvectionAlgorithm:Inside,TARP;",

        "  SurfaceConvectionAlgorithm:Outside,DOE-2;",

        "  HeatBalanceAlgorithm,ConductionTransferFunction;",

        "  Schedule:Constant,FanAndCoilAvailSched,,1.0;",
        "  Schedule:Constant,On,,1.0;",
        "  Schedule:Constant,WindowVentSched,,21.0;",
        "  Schedule:Constant,VentingSched,,0.0;",

        "  Site:GroundTemperature:BuildingSurface,20.03,20.03,20.13,20.30,20.43,20.52,20.62,20.77,20.78,20.55,20.44,20.20;",

        "  Material,",
        "    A1 - 1 IN STUCCO,        !- Name",
        "    Smooth,                  !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    0.6918309,               !- Conductivity {W/m-K}",
        "    1858.142,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Material,",
        "    C4 - 4 IN COMMON BRICK,  !- Name",
        "    Rough,                   !- Roughness",
        "    0.1014984,               !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1922.216,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7600000,               !- Solar Absorptance",
        "    0.7600000;               !- Visible Absorptance",

        "  Material,",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Name",
        "    Smooth,                  !- Roughness",
        "    1.9050000E-02,           !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1601.846,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Material,",
        "    C6 - 8 IN CLAY TILE,     !- Name",
        "    Smooth,                  !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    0.5707605,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.8200000,               !- Solar Absorptance",
        "    0.8200000;               !- Visible Absorptance",

        "  Material,",
        "    C10 - 8 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "  Material,",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Name",
        "    Rough,                   !- Roughness",
        "    1.2710161E-02,           !- Thickness {m}",
        "    1.435549,                !- Conductivity {W/m-K}",
        "    881.0155,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5500000,               !- Solar Absorptance",
        "    0.5500000;               !- Visible Absorptance",

        "  Material,",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Name",
        "    Rough,                   !- Roughness",
        "    9.5402403E-03,           !- Thickness {m}",
        "    0.1902535,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "  Material,",
        "    B5 - 1 IN DENSE INSULATION,  !- Name",
        "    VeryRough,               !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    4.3239430E-02,           !- Conductivity {W/m-K}",
        "    91.30524,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5000000,               !- Solar Absorptance",
        "    0.5000000;               !- Visible Absorptance",

        "  Material,",
        "    C12 - 2 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    5.0901599E-02,           !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "  Material,",
        "    1.375in-Solid-Core,      !- Name",
        "    Smooth,                  !- Roughness",
        "    3.4925E-02,              !- Thickness {m}",
        "    0.1525000,               !- Conductivity {W/m-K}",
        "    614.5000,                !- Density {kg/m3}",
        "    1630.0000,               !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Construction,",
        "    EXTWALL80,               !- Name",
        "    A1 - 1 IN STUCCO,        !- Outside Layer",
        "    C4 - 4 IN COMMON BRICK,  !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "  Construction,",
        "    PARTITION06,             !- Name",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "    C6 - 8 IN CLAY TILE,     !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "  Construction,",
        "    FLOOR SLAB 8 IN,         !- Name",
        "    C10 - 8 IN HW CONCRETE;  !- Outside Layer",

        "  Construction,",
        "    ROOF34,                  !- Name",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Outside Layer",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Layer 2",
        "    C12 - 2 IN HW CONCRETE;  !- Layer 3",

        "  Construction,",
        "    CEILING:ZONE,            !- Name",
        "    B5 - 1 IN DENSE INSULATION,  !- Outside Layer",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 2",

        "  Construction,",
        "    CEILING:ATTIC,           !- Name",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "    B5 - 1 IN DENSE INSULATION;  !- Layer 2",

        "  Construction,",
        "    DOOR-CON,                !- Name",
        "    1.375in-Solid-Core;      !- Outside Layer",

        " Construction,",
        "  window - 90.1 - 2004 - nonres - fixed, !- Name",
        "  ASHRAE NonRes Fixed Assembly Window;  !- Outside Layer",

        " WindowMaterial:SimpleGlazingSystem,",
        "  ASHRAE NonRes Fixed Assembly Window, !- Name",
        "  3.23646, !- U - Factor{ W / m2 - K }",
        "  0.39, !- Solar Heat Gain Coefficient",
        "  ;                        !- Visible Transmittance",

        "  Zone,",
        "    West Zone,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    EAST ZONE,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    NORTH ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",
        "  Zone,",
        "    ATTIC ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  GlobalGeometryRules,",
        "    UpperLeftCorner,         !- Starting Vertex Position",
        "    CounterClockWise,        !- Vertex Entry Direction",
        "    World;                   !- Coordinate System",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Flr002,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Wall003,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Flr003,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall006,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall007,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall008,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Roof001,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Roof002,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Roof003,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Flr002,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Flr003,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn001:Wall001:Win001,    !- Name",
        "    Window,                  !- Surface Type",
        "    window - 90.1 - 2004 - nonres - fixed,  !- Construction Name",
        "    Zn001:Wall001,           !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    0.548000,0,2.5000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0.548000,0,0.5000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    5.548000,0,0.5000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    5.548000,0,2.5000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn001:Wall003:Door001,   !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Zn001:Wall003,           !- Building Surface Name",
        "    Zn003:Wall004:Door001,   !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    3.500,6.096000,2.0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    3.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    2.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    2.500,6.096000,2.0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn003:Wall002:Win001,    !- Name",
        "    Window,                  !- Surface Type",
        "    window - 90.1 - 2004 - nonres - fixed,  !- Construction Name",
        "    Zn003:Wall002,           !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    5.548000,12.19200,2.5000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    5.548000,12.19200,0.5000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0.548000,12.19200,0.5000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0.548000,12.19200,2.5000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn003:Wall004:Door001,   !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Zn003:Wall004,           !- Building Surface Name",
        "    Zn001:Wall003:Door001,   !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    2.500,6.096000,2.0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    2.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    3.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    3.500,6.096000,2.0;  !- X,Y,Z ==> Vertex 4 {m}",
        "	OutdoorAir:Mixer,",
        "	OA Mixing Box 1, !- Name",
        "	Mixed Air Node, !- Mixed Air Node Name",
        "	Outside Air Inlet Node, !- Outdoor Air Stream Node Name",
        "	Relief Air Outlet Node, !- Relief Air Stream Node Name",
        "	Air Loop Inlet Node;     !- Return Air Stream Node Name",
        "  AirflowNetwork:SimulationControl,",
        "    AriflowNetwork_All,      !- Name",
        "    MultizoneWithDistribution,  !- AirflowNetwork Control",
        "    INPUT,                   !- Wind Pressure Coefficient Type",
        "    ExternalNode,            !- Height Selection for Local Wind Pressure Calculation",
        "    LOWRISE,                 !- Building Type",
        "    500,                     !- Maximum Number of Iterations {dimensionless}",
        "    ZeroNodePressures,       !- Initialization Type",
        "    1.0E-05,                 !- Relative Airflow Convergence Tolerance {dimensionless}",
        "    1.0E-06,                 !- Absolute Airflow Convergence Tolerance {kg/s}",
        "    -0.5,                    !- Convergence Acceleration Limit {dimensionless}",
        "    0.0,                     !- Azimuth Angle of Long Axis of Building {deg}",
        "    1.0;                     !- Ratio of Building Width Along Short Axis to Width Along Long Axis",

        "  AirflowNetwork:MultiZone:Zone,",
        "    West Zone,               !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    0.3,                     !- Minimum Venting Open Factor {dimensionless}",
        "    5.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    10.0,                    !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0,                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",
        "    VentingSched;            !- Venting Availability Schedule Name",

        "  AirflowNetwork:MultiZone:Zone,",
        "    EAST ZONE,               !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    NORTH ZONE,              !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0,                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",
        "    VentingSched;            !- Venting Availability Schedule Name",

        "  AirflowNetwork:MultiZone:Zone,",
        "    ATTIC ZONE,              !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall001,           !- Surface Name",
        "    ELA-1,                   !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall001:Win001,    !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall003,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall003:Door001,   !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall004,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall003,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    EFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall005,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall002:Win001,    !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall003,           !- Surface Name",
        "    Zone3 Exhaust Fan,       !- Leakage Component Name",
        "    EFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn004:Roof001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn004:Roof002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn004:Roof003,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "    ReferenceCrackConditions,!- Name",
        "    20.0,                    !- Reference Temperature {C}",
        "    101325,                  !- Reference Barometric Pressure {Pa}",
        "    0.0;                     !- Reference Humidity Ratio {kgWater/kgDryAir}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CR-1,                    !- Name",
        "    0.01,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CRcri,                   !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Component:ZoneExhaustFan,",
        "    Zone3 Exhaust Fan,       !- Name",
        "    0.01,                    !- Air Mass Flow Coefficient When the Zone Exhaust Fan is Off at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When the Zone Exhaust Fan is Off {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:EffectiveLeakageArea,",
        "    ELA-1,                   !- Name",
        "    0.007,                   !- Effective Leakage Area {m2}",
        "    1.0,                     !- Discharge Coefficient {dimensionless}",
        "    4.0,                     !- Reference Pressure Difference {Pa}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    NFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    NFacade_WPCValue;        !- Wind Pressure Coefficient Values Object Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    EFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    EFacade_WPCValue;        !- Wind Pressure Coefficient Values Object Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    SFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    SFacade_WPCValue;        !- Wind Pressure Coefficient Values Object Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    WFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    WFacade_WPCValue;        !- Wind Pressure Coefficient Values Object Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    Horizontal,              !- Name",
        "    3.028,                   !- External Node Height {m}",
        "    Horizontal_WPCValue;     !- Wind Pressure Coefficient Values Object Name",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientArray,",
        "    Every 30 Degrees,        !- Name",
        "    0,                       !- Wind Direction 1 {deg}",
        "    30,                      !- Wind Direction 2 {deg}",
        "    60,                      !- Wind Direction 3 {deg}",
        "    90,                      !- Wind Direction 4 {deg}",
        "    120,                     !- Wind Direction 5 {deg}",
        "    150,                     !- Wind Direction 6 {deg}",
        "    180,                     !- Wind Direction 7 {deg}",
        "    210,                     !- Wind Direction 8 {deg}",
        "    240,                     !- Wind Direction 9 {deg}",
        "    270,                     !- Wind Direction 10 {deg}",
        "    300,                     !- Wind Direction 11 {deg}",
        "    330;                     !- Wind Direction 12 {deg}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    NFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.60,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.48;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    EFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.56;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    SFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.37,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.42;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    WFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.04;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    Horizontal_WPCValue,     !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.00,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.00;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentInletNode,      !- Name",
        "    Zone Equipment Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentOutletNode,     !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    SupplyMainNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainSplitterNode,        !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ReheatInlet1Node,        !- Name",
        "    Zone 1 Reheat Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1SupplyRegisterNode, !- Name",
        "    Zone 1 Reheat Air Outlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1OutletNode,         !- Name",
        "    Zone 1 Outlet Node,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ReheatInlet2Node,        !- Name",
        "    Zone 2 Reheat Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2SupplyRegisterNode, !- Name",
        "    Zone 2 Reheat Air Outlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2OutletNode,         !- Name",
        "    Zone 2 Outlet Node,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone3SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone3SupplyRegisterNode, !- Name",
        "    Zone 3 Inlet Node,       !- Component Name or Node Name",
        "    ,                        !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone3OutletNode,         !- Name",
        "    Zone 3 Outlet Node,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone3ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainMixerNode,           !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainReturnNode,          !- Name",
        "    Return Air Mixer Outlet, !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainInletNode,           !- Name",
        "    Air Loop Inlet Node,     !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    OA System Node,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:OutdoorAirSystem,  !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    OA Inlet Node,           !- Name",
        "    Outside Air Inlet Node,  !- Component Name or Node Name",
        "    OAMixerOutdoorAirStreamNode,  !- Component Object Type or Node Type",
        "    1.5;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanInletNode,            !- Name",
        "    Mixed Air Node,          !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanOutletNode,           !- Name",
        "    Cooling Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingInletNode,        !- Name",
        "    Heating Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingOutletNode,       !- Name",
        "    Air Loop Outlet Node,    !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Component:Leak,",
        "    MainSupplyLeak,          !- Name",
        "    0.0025,                  !- Air Mass Flow Coefficient {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:ConstantPressureDrop,",
        "    SupplyCPDComp,           !- Name",
        "    1.0;                     !- Pressure Difference Across the Component {Pa}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ZoneSupplyELR1,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ZoneSupplyELR2,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ZoneSupplyELR3,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeakELR1,          !- Name",
        "    0.03,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    41.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeakELR2,          !- Name",
        "    0.03,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    40.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeakELR3,          !- Name",
        "    0.04,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    43.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTruck1,              !- Name",
        "    3.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTruck2,              !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone1Supply,             !- Name",
        "    5.0,                     !- Duct Length {m}",
        "    0.4,                     !- Hydraulic Diameter {m}",
        "    0.1256,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone2Supply,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.39,                    !- Hydraulic Diameter {m}",
        "    0.1195,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    2.5,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone3Supply,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.44,                    !- Hydraulic Diameter {m}",
        "    0.1521,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone1Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone2Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.48,                    !- Hydraulic Diameter {m}",
        "    0.1809,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone3Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.55,                    !- Hydraulic Diameter {m}",
        "    0.2376,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.772,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneConnectionDuct,      !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    30.00,                   !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MixerConnectionDuct,     !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopReturn,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopSupply,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001,                   !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0001;                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Fan,",
        "    Supply Fan 1,            !- Fan Name",
        "    Fan:ConstantVolume;      !- Supply Fan Object Type",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    ACDXCoil 1,              !- Coil Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Main Heating Coil 1,     !- Coil Name",
        "    Coil:Heating:Fuel,        !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:TerminalUnit,",
        "    Reheat Zone 1,           !- Terminal Unit Name",
        "    AirTerminal:SingleDuct:ConstantVolume:Reheat,  !- Terminal Unit Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    0.44;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:TerminalUnit,",
        "    Reheat Zone 2,           !- Terminal Unit Name",
        "    AirTerminal:SingleDuct:ConstantVolume:Reheat,  !- Terminal Unit Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    0.44;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Leak,",
        "    OAFlow,          !- Name",
        "    0.025,                  !- Air Mass Flow Coefficient {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:Leak,",
        "    OAFlow1,          !- Name",
        "    0.025,                  !- Air Mass Flow Coefficient {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link 1,             !- Name",
        "    EquipmentInletNode,      !- Node 1 Name",
        "    EquipmentOutletNode,     !- Node 2 Name",
        "    MainTruck1,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main CDP Link,           !- Name",
        "    EquipmentOutletNode,     !- Node 1 Name",
        "    SupplyMainNode,          !- Node 2 Name",
        "    SupplyCPDComp;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link 2,             !- Name",
        "    SupplyMainNode,          !- Node 1 Name",
        "    MainSplitterNode,        !- Node 2 Name",
        "    MainTruck2,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Supply1Link,        !- Name",
        "    MainSplitterNode,        !- Node 1 Name",
        "    Zone1SupplyNode,         !- Node 2 Name",
        "    Zone1Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Supply2Link,        !- Name",
        "    Zone1SupplyNode,         !- Node 1 Name",
        "    ReheatInlet1Node,        !- Node 2 Name",
        "    Zone1Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1ReheatCoilLink,     !- Name",
        "    ReheatInlet1Node,        !- Node 1 Name",
        "    Zone1SupplyRegisterNode, !- Node 2 Name",
        "    Reheat Zone 1;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1SupplyConnectionLink,  !- Name",
        "    Zone1SupplyRegisterNode, !- Node 1 Name",
        "    West Zone,               !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1ReturnConnectionLink,  !- Name",
        "    West Zone,               !- Node 1 Name",
        "    Zone1OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Supply1Link,        !- Name",
        "    MainSplitterNode,        !- Node 1 Name",
        "    Zone2SupplyNode,         !- Node 2 Name",
        "    Zone2Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Supply2Link,        !- Name",
        "    Zone2SupplyNode,         !- Node 1 Name",
        "    ReheatInlet2Node,        !- Node 2 Name",
        "    Zone2Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2ReheatCoilLink,     !- Name",
        "    ReheatInlet2Node,        !- Node 1 Name",
        "    Zone2SupplyRegisterNode, !- Node 2 Name",
        "    Reheat Zone 2;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2SupplyConnectionLink,  !- Name",
        "    Zone2SupplyRegisterNode, !- Node 1 Name",
        "    EAST ZONE,               !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2returnConnectionLink,  !- Name",
        "    EAST ZONE,               !- Node 1 Name",
        "    Zone2OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3Supply1Link,        !- Name",
        "    MainSplitterNode,        !- Node 1 Name",
        "    Zone3SupplyNode,         !- Node 2 Name",
        "    Zone3Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3SupplyLink,         !- Name",
        "    Zone3SupplyNode,         !- Node 1 Name",
        "    Zone3SupplyRegisterNode, !- Node 2 Name",
        "    Zone3Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3SupplyConnectionLink,  !- Name",
        "    Zone3SupplyRegisterNode, !- Node 1 Name",
        "    NORTH ZONE,              !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3ReturnConnectionLink,  !- Name",
        "    NORTH ZONE,              !- Node 1 Name",
        "    Zone3OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Return1Link,        !- Name",
        "    Zone1OutletNode,         !- Node 1 Name",
        "    Zone1ReturnNode,         !- Node 2 Name",
        "    Zone1Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Return2Link,        !- Name",
        "    Zone1ReturnNode,         !- Node 1 Name",
        "    MainMixerNode,           !- Node 2 Name",
        "    Zone1Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Return1Link,        !- Name",
        "    Zone2OutletNode,         !- Node 1 Name",
        "    Zone2ReturnNode,         !- Node 2 Name",
        "    Zone2Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Return2Link,        !- Name",
        "    Zone2ReturnNode,         !- Node 1 Name",
        "    MainMixerNode,           !- Node 2 Name",
        "    Zone2Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3Return1Link,        !- Name",
        "    Zone3OutletNode,         !- Node 1 Name",
        "    Zone3ReturnNode,         !- Node 2 Name",
        "    Zone3Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3Return2Link,        !- Name",
        "    Zone3ReturnNode,         !- Node 1 Name",
        "    MainMixerNode,           !- Node 2 Name",
        "    Zone3Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ReturnMixerLink,         !- Name",
        "    MainMixerNode,           !- Node 1 Name",
        "    MainReturnNode,          !- Node 2 Name",
        "    MixerConnectionDuct,     !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    AirLoopReturnLink,       !- Name",
        "    MainReturnNode,          !- Node 1 Name",
        "    MainInletNode,           !- Node 2 Name",
        "    AirLoopReturn;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemInletLink,       !- Name",
        "    MainInletNode,           !- Node 1 Name",
        "    OA System Node,          !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemFanLink,         !- Name",
        "    OA Inlet Node,           !- Node 1 Name",
        "    OA System Node,          !- Node 2 Name",
        "    OA Fan;                  !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemReliefLink,      !- Name",
        "    OA System Node,          !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    Relief Fan;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OAMixerOutletLink,       !- Name",
        "    OA System Node,          !- Node 1 Name",
        "    FanInletNode,            !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SupplyFanLink,           !- Name",
        "    FanInletNode,            !- Node 1 Name",
        "    FanOutletNode,           !- Node 2 Name",
        "    Supply Fan 1;            !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    CoolingCoilLink,         !- Name",
        "    FanOutletNode,           !- Node 1 Name",
        "    HeatingInletNode,        !- Node 2 Name",
        "    ACDXCoil 1;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    HeatingCoilLink,         !- Name",
        "    HeatingInletNode,        !- Node 1 Name",
        "    HeatingOutletNode,       !- Node 2 Name",
        "    Main Heating Coil 1;     !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    EquipmentAirLoopLink,    !- Name",
        "    HeatingOutletNode,       !- Node 1 Name",
        "    EquipmentInletNode,      !- Node 2 Name",
        "    AirLoopSupply;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1ReturnLeakLink,     !- Name",
        "    Zone1ReturnNode,         !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    ReturnLeakELR1;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    MainSupplyLeakLink,      !- Name",
        "    SupplyMainNode,          !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    MainSupplyLeak;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1SupplyLeakLink,     !- Name",
        "    Zone1SupplyNode,         !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    ZoneSupplyELR1;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2ReturnLeakLink,     !- Name",
        "    Zone2ReturnNode,         !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    ReturnLeakELR2;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3ReturnLeakLink,     !- Name",
        "    Zone3ReturnNode,         !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    ReturnLeakELR3;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2SupplyLeakLink,     !- Name",
        "    Zone2SupplyNode,         !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    ZoneSupplyELR2;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone3SupplyLeakLink,     !- Name",
        "    Zone3SupplyNode,         !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    ZoneSupplyELR3;          !- Component Name",

        "  AirflowNetwork:ZoneControl:PressureController,",
        "    PressureController,      !- Name",
        "    NORTH ZONE,              !- Control Zone Name",
        "    AirflowNetwork:Distribution:Component:ReliefAirFlow, !- Control Object Type",
        "    Relief Fan,              !- Control Object Name",
        "    ,                        !- Pressure Control Availability Schedule Name",
        "    Pressure Setpoint Schedule; !- Pressure Setpoint Schedule Name",

        "  Schedule:Compact,",
        "    Pressure Setpoint Schedule,   !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.5,        !- Field 3",
        "    Through: 9/30,           !- Field 4",
        "    For: AllDays,            !- Field 5",
        "    Until: 24:00,3.5,        !- Field 6",
        "    Through: 12/31,          !- Field 7",
        "    For: AllDays,            !- Field 8",
        "    Until: 24:00,0.5;        !- Field 9",

        "  AirflowNetwork:Distribution:Component:OutdoorAirFlow,",
        "    OA Fan,                  !- Name",
        "	 OA Mixing Box 1, !- Outdoor Air Mixer Name",
        "    0.001,                   !- Air Mass Flow Coefficient When the Zone Exhaust Fan is Off at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When the Zone Exhaust Fan is Off {dimensionless}",

        "  AirflowNetwork:Distribution:Component:ReliefAirFlow,",
        "    Relief Fan,              !- Name",
        "	 OA Mixing Box 1, !- Outdoor Air Mixer Name",
        "    0.001,                   !- Air Mass Flow Coefficient When the Zone Exhaust Fan is Off at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When the Zone Exhaust Fan is Off {dimensionless}",

        "  AirLoopHVAC,",
        "    Typical Terminal Reheat 1,  !- Name",
        "    ,                        !- Controller List Name",
        "    Reheat System 1 Avail List,  !- Availability Manager List Name",
        "    1.9,                     !- Design Supply Air Flow Rate {m3/s}",
        "    Air Loop Branches,       !- Branch List Name",
        "    ,                        !- Connector List Name",
        "    Air Loop Inlet Node,     !- Supply Side Inlet Node Name",
        "    Return Air Mixer Outlet, !- Demand Side Outlet Node Name",
        "    Zone Equipment Inlet Node,  !- Demand Side Inlet Node Names",
        "    Air Loop Outlet Node;    !- Supply Side Outlet Node Names",

        "  Fan:ZoneExhaust,",
        "    Zone3 Exhaust Fan,       !- Name",
        "    On,    !- Availability Schedule Name",
        "    0.7,                     !- Fan Total Efficiency",
        "    500,                     !- Pressure Rise {Pa}",
        "    0.01,                    !- Maximum Flow Rate {m3/s}",
        "    Zone3 Exhaust Node,      !- Air Inlet Node Name",
        "    Zone3 Exhaust Fan Outlet Node,  !- Air Outlet Node Name",
        "    Zone Exhaust;            !- End-Use Subcategory",

        "  Fan:ConstantVolume,",
        "    Supply Fan 1,            !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    0.7,                     !- Fan Total Efficiency",
        "    600.0,                   !- Pressure Rise {Pa}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    0.9,                     !- Motor Efficiency",
        "    1.0,                     !- Motor In Airstream Fraction",
        "    Mixed Air Node,          !- Air Inlet Node Name",
        "    Cooling Coil Air Inlet Node;  !- Air Outlet Node Name",

    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataIPShortCut->lNumericFieldBlanks.allocate(1000);
    state->dataIPShortCut->lAlphaFieldBlanks.allocate(1000);
    state->dataIPShortCut->cAlphaFieldNames.allocate(1000);
    state->dataIPShortCut->cNumericFieldNames.allocate(1000);
    state->dataIPShortCut->cAlphaArgs.allocate(1000);
    state->dataIPShortCut->rNumericArgs.allocate(1000);
    state->dataIPShortCut->lNumericFieldBlanks = false;
    state->dataIPShortCut->lAlphaFieldBlanks = false;
    state->dataIPShortCut->cAlphaFieldNames = " ";
    state->dataIPShortCut->cNumericFieldNames = " ";
    state->dataIPShortCut->cAlphaArgs = " ";
    state->dataIPShortCut->rNumericArgs = 0.0;

    bool ErrorsFound = false;
    // Read objects
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetWindowGlassSpectralData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    Material::GetMaterialData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    SurfaceGeometry::GetGeometryParameters(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosBldgRotAppGonly = 1.0;
    state->dataSurfaceGeometry->SinBldgRotAppGonly = 0.0;
    SurfaceGeometry::GetSurfaceData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    // Read AirflowNetwork inputs
    state->afn->get_input();

    Real64 PressureSet = 0.5;

    state->dataScheduleMgr
        ->Schedule(UtilityRoutines::FindItemInList("PRESSURE SETPOINT SCHEDULE",
                                                   state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules})))
        .CurrentValue = PressureSet; // Pressure setpoint
    state->dataScheduleMgr
        ->Schedule(
            UtilityRoutines::FindItemInList("FANANDCOILAVAILSCHED", state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules})))
        .CurrentValue = 1.0; // set availability and fan schedule to 1
    state->dataScheduleMgr
        ->Schedule(UtilityRoutines::FindItemInList("ON", state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules})))
        .CurrentValue = 1.0; // On
    state->dataScheduleMgr
        ->Schedule(UtilityRoutines::FindItemInList("VENTINGSCHED", state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules})))
        .CurrentValue = 25.55; // VentingSched
    state->dataScheduleMgr
        ->Schedule(UtilityRoutines::FindItemInList("WINDOWVENTSCHED", state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules})))
        .CurrentValue = 1.0; // WindowVentSched

    state->afn->AirflowNetworkFanActivated = true;
    state->dataEnvrn->OutDryBulbTemp = -17.29025;
    state->dataEnvrn->OutHumRat = 0.0008389;
    state->dataEnvrn->OutBaroPress = 99063.0;
    state->dataEnvrn->WindSpeed = 4.9;
    state->dataEnvrn->WindDir = 270.0;
    state->dataEnvrn->StdRhoAir = 1.2;

    int index = UtilityRoutines::FindItemInList("OA INLET NODE", state->afn->AirflowNetworkNodeData);
    for (i = 1; i <= 36; ++i) {
        state->afn->AirflowNetworkNodeSimu(i).TZ = 23.0;
        state->afn->AirflowNetworkNodeSimu(i).WZ = 0.0008400;
        if ((i > 4 && i < 10) || i == index) { // NFACADE, EFACADE, SFACADE, WFACADE, HORIZONTAL are always at indexes 5 through 9
            state->afn->AirflowNetworkNodeSimu(i).TZ =
                DataEnvironment::OutDryBulbTempAt(*state, state->afn->AirflowNetworkNodeData(i).NodeHeight); // AirflowNetworkNodeData vals differ
            state->afn->AirflowNetworkNodeSimu(i).WZ = state->dataEnvrn->OutHumRat;
        }
    }

    // Set up node values
    state->dataLoopNodes->Node.allocate(10);
    if (state->afn->MultizoneCompExhaustFanData(1).InletNode == 0) {
        state->afn->MultizoneCompExhaustFanData(1).InletNode = 3;
    }
    state->dataLoopNodes->Node(state->afn->MultizoneCompExhaustFanData(1).InletNode).MassFlowRate = 0.1005046;

    if (state->afn->DisSysCompCVFData(1).InletNode == 0) {
        state->afn->DisSysCompCVFData(1).InletNode = 1;
    }
    state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(1).InletNode).MassFlowRate = 2.23418088;
    state->afn->DisSysCompCVFData(1).FlowRate = state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(1).InletNode).MassFlowRate;

    if (state->afn->DisSysCompOutdoorAirData(1).InletNode == 0) {
        state->afn->DisSysCompOutdoorAirData(1).InletNode = 5;
        state->afn->DisSysCompOutdoorAirData(1).OutletNode = 6;
    }
    state->dataLoopNodes->Node(state->afn->DisSysCompOutdoorAirData(1).InletNode).MassFlowRate = 0.5095108;
    state->dataLoopNodes->Node(state->afn->DisSysCompOutdoorAirData(1).OutletNode).MassFlowRate = 0.5095108;

    if (state->afn->DisSysCompReliefAirData(1).InletNode == 0) {
        state->afn->DisSysCompReliefAirData(1).InletNode = 6;
        state->afn->DisSysCompReliefAirData(1).OutletNode = 5;
    }
    state->afn->AirflowNetworkNodeData(3).AirLoopNum = 1;
    state->afn->AirflowNetworkLinkageData(46).AirLoopNum = 1;

    state->dataAirLoop->AirLoopAFNInfo.allocate(1);
    //    state->dataAirLoop->LoopOnOffFanPartLoadRatio.allocate(1);
    state->dataAirLoop->AirLoopAFNInfo(1).LoopFanOperationMode = 0.0;
    state->dataAirLoop->AirLoopAFNInfo(1).LoopOnOffFanPartLoadRatio = 0.0;
    // Calculate mass flow rate based on pressure setpoint
    state->afn->PressureControllerData(1).OANodeNum = state->afn->DisSysCompReliefAirData(1).OutletNode;
    state->afn->ANZT = 26.0;
    state->afn->ANZW = 0.0011;
    state->afn->calculate_balance();

    // Check indoor pressure and mass flow rate
    EXPECT_NEAR(PressureSet, state->afn->AirflowNetworkNodeSimu(3).PZ, 0.0001);
    EXPECT_NEAR(0.00255337, state->afn->ReliefMassFlowRate, 0.0001);

    // Start a test for #5687 to report zero values of AirflowNetwork:Distribution airflow and pressure outputs when a system is off
    state->afn->AirflowNetworkFanActivated = false;

    state->afn->exchangeData.allocate(state->dataGlobal->NumOfZones);

    state->afn->update();

    EXPECT_NEAR(0.0, state->afn->AirflowNetworkNodeSimu(10).PZ, 0.0001);
    EXPECT_NEAR(0.0, state->afn->AirflowNetworkNodeSimu(20).PZ, 0.0001);
    EXPECT_NEAR(0.0, state->afn->linkReport(20).FLOW, 0.0001);
    EXPECT_NEAR(0.0, state->afn->linkReport(50).FLOW, 0.0001);

    // Start a test for #6005
    state->afn->MultizoneSurfaceData(2).HybridVentClose = true;
    state->afn->MultizoneSurfaceData(5).HybridVentClose = true;
    state->afn->MultizoneSurfaceData(14).HybridVentClose = true;
    state->afn->calculate_balance();
    EXPECT_EQ(0.0, state->afn->MultizoneSurfaceData(2).OpenFactor);
    EXPECT_EQ(0.0, state->afn->MultizoneSurfaceData(5).OpenFactor);
    EXPECT_EQ(0.0, state->afn->MultizoneSurfaceData(14).OpenFactor);
    EXPECT_EQ(0.0, state->dataSurface->SurfWinVentingOpenFactorMultRep(2));
    EXPECT_EQ(0.0, state->dataSurface->SurfWinVentingOpenFactorMultRep(5));
    EXPECT_EQ(0.0, state->dataSurface->SurfWinVentingOpenFactorMultRep(14));

    // Test for #7162
    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(state->dataGlobal->NumOfZones);
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(2).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(3).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(4).MAT = 5.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZoneAirHumRat = 0.0007;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(2).ZoneAirHumRat = 0.0011;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(3).ZoneAirHumRat = 0.0012;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(4).ZoneAirHumRat = 0.0008;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZoneAirHumRatAvg = 0.0007;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(2).ZoneAirHumRatAvg = 0.0011;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(3).ZoneAirHumRatAvg = 0.0012;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(4).ZoneAirHumRatAvg = 0.0008;
    state->dataZoneEquip->ZoneEquipConfig.allocate(4);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = false;
    state->dataZoneEquip->ZoneEquipConfig(2).IsControlled = false;
    state->dataZoneEquip->ZoneEquipConfig(3).IsControlled = false;
    state->dataZoneEquip->ZoneEquipConfig(4).IsControlled = false;
    state->dataHVACGlobal->TimeStepSys = 0.1;
    state->dataHVACGlobal->TimeStepSysSec = state->dataHVACGlobal->TimeStepSys * Constant::SecInHour;

    state->afn->AirflowNetworkLinkSimu(1).FLOW2 = 0.1;
    state->afn->AirflowNetworkLinkSimu(10).FLOW2 = 0.15;
    state->afn->AirflowNetworkLinkSimu(13).FLOW2 = 0.1;

    state->afn->report();

    // Original results
    // EXPECT_NEAR(34.3673036, state->afn->AirflowNetworkReportData(1).MultiZoneInfiLatGainW, 0.0001);
    // EXPECT_NEAR(36.7133377, state->afn->AirflowNetworkReportData(2).MultiZoneMixLatGainW, 0.0001);
    // EXPECT_NEAR(89.3450925, state->afn->AirflowNetworkReportData(3).MultiZoneInfiLatLossW, 0.0001);
    // revised based #7844
    EXPECT_NEAR(35.3319353, state->afn->AirflowNetworkReportData(1).MultiZoneInfiLatGainW, 0.0001);
    EXPECT_NEAR(38.1554377, state->afn->AirflowNetworkReportData(2).MultiZoneMixLatGainW, 0.0001);
    EXPECT_NEAR(91.8528571, state->afn->AirflowNetworkReportData(3).MultiZoneInfiLatLossW, 0.0001);

    auto &thisZoneHB = state->dataZoneTempPredictorCorrector->zoneHeatBalance(1);
    Real64 hg = Psychrometrics::PsyHgAirFnWTdb(thisZoneHB.ZoneAirHumRat, thisZoneHB.MAT);
    Real64 hzone = Psychrometrics::PsyHFnTdbW(thisZoneHB.MAT, thisZoneHB.ZoneAirHumRat);
    Real64 hamb = Psychrometrics::PsyHFnTdbW(0.0, state->dataEnvrn->OutHumRat);
    Real64 hdiff = state->afn->AirflowNetworkLinkSimu(1).FLOW2 * (hzone - hamb);
    Real64 sum = state->afn->AirflowNetworkReportData(1).MultiZoneInfiSenLossW - state->afn->AirflowNetworkReportData(1).MultiZoneInfiLatGainW;
    // Existing code uses T_average to calculate hg, get close results
    EXPECT_NEAR(hdiff, sum, 0.4);
    Real64 dhlatent = state->afn->AirflowNetworkLinkSimu(1).FLOW2 * hg * (thisZoneHB.ZoneAirHumRat - state->dataEnvrn->OutHumRat);
    // when hg is calculated with indoor temperature, get exact results
    sum = state->afn->AirflowNetworkReportData(1).MultiZoneInfiSenLossW + dhlatent;
    EXPECT_NEAR(hdiff, sum, 0.001);
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestZoneVentingSchWithAdaptiveCtrl)
{

    // Unit test for #5490

    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "SOFF";

    state->dataSurface->Surface.allocate(2);
    state->dataSurface->Surface(1).Name = "WINDOW 1";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "SOFF";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 90.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(2).Name = "WINDOW 2";
    state->dataSurface->Surface(2).Zone = 1;
    state->dataSurface->Surface(2).ZoneName = "SOFF";
    state->dataSurface->Surface(2).Azimuth = 180.0;
    state->dataSurface->Surface(2).ExtBoundCond = 0;
    state->dataSurface->Surface(2).HeatTransSurf = true;
    state->dataSurface->Surface(2).Tilt = 90.0;
    state->dataSurface->Surface(2).Sides = 4;

    SurfaceGeometry::AllocateSurfaceWindows(*state, 2);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;
    state->dataSurface->SurfWinOriginalClass(2) = DataSurfaces::SurfaceClass::Window;
    state->dataGlobal->NumOfZones = 1;

    state->dataHeatBal->TotPeople = 1; // Total number of people statements
    state->dataHeatBal->People.allocate(state->dataHeatBal->TotPeople);
    state->dataHeatBal->People(1).ZonePtr = 1;
    state->dataHeatBal->People(1).NumberOfPeople = 100.0;
    state->dataHeatBal->People(1).NumberOfPeoplePtr = 1; // From dataglobals, always returns a 1 for schedule value
    state->dataHeatBal->People(1).AdaptiveCEN15251 = true;

    std::string const idf_objects = delimited_string({
        "Schedule:Constant,OnSch,,1.0;",
        "Schedule:Constant,FreeRunningSeason,,0.0;",
        "Schedule:Constant,Sempre 21,,21.0;",
        "AirflowNetwork:SimulationControl,",
        "  NaturalVentilation, !- Name",
        "  MultizoneWithoutDistribution, !- AirflowNetwork Control",
        "  SurfaceAverageCalculation, !- Wind Pressure Coefficient Type",
        "  , !- Height Selection for Local Wind Pressure Calculation",
        "  LOWRISE, !- Building Type",
        "  1000, !- Maximum Number of Iterations{ dimensionless }",
        "  ZeroNodePressures, !- Initialization Type",
        "  0.0001, !- Relative Airflow Convergence Tolerance{ dimensionless }",
        "  0.0001, !- Absolute Airflow Convergence Tolerance{ kg / s }",
        "  -0.5, !- Convergence Acceleration Limit{ dimensionless }",
        "  90, !- Azimuth Angle of Long Axis of Building{ deg }",
        "  0.36;                    !- Ratio of Building Width Along Short Axis to Width Along Long Axis",
        "AirflowNetwork:MultiZone:Zone,",
        "  Soff, !- Zone Name",
        "  CEN15251Adaptive, !- Ventilation Control Mode",
        "  , !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "  , !- Minimum Venting Open Factor{ dimensionless }",
        "  , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "  100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "  , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "  300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "  FreeRunningSeason; !- Venting Availability Schedule Name",
        "AirflowNetwork:MultiZone:Surface,",
        "  window 1, !- Surface Name",
        "  Simple Window, !- Leakage Component Name",
        "  , !- External Node Name",
        "  1, !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "  ZoneLevel; !- Ventilation Control Mode",
        "AirflowNetwork:MultiZone:Surface,",
        "  window 2, !- Surface Name",
        "  Simple Window, !- Leakage Component Name",
        "  , !- External Node Name",
        "  1, !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "  ZoneLevel; !- Ventilation Control Mode",
        "AirflowNetwork:MultiZone:Component:SimpleOpening,",
        "  Simple Window, !- Name",
        "  0.0010, !- Air Mass Flow Coefficient When Opening is Closed{ kg / s - m }",
        "  0.65, !- Air Mass Flow Exponent When Opening is Closed{ dimensionless }",
        "  0.01, !- Minimum Density Difference for Two - Way Flow{ kg / m3 }",
        "  0.78;                    !- Discharge Coefficient{ dimensionless }",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->afn->get_input();

    // The original value before fix is zero. After the fix, the correct schedule number is assigned.

    // changed index 2 to 1 because in new sorted scheedule MultizoneZone(1).VentingSchName ("FREERUNNINGSEASON")
    // has index 1 which is the .VentSchNum
    auto GetIndex = UtilityRoutines::FindItemInList(state->afn->MultizoneZoneData(1).VentingSchName,
                                                    state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules}));
    EXPECT_EQ(GetIndex, state->afn->MultizoneZoneData(1).VentingSchNum);

    state->dataHeatBal->Zone.deallocate();
    state->dataSurface->Surface.deallocate();
    state->dataSurface->SurfaceWindow.deallocate();
    state->dataHeatBal->People.deallocate();
}

TEST_F(EnergyPlusFixture, AirflowNetwork_MultiAirLoopTest)
{

    std::string const idf_objects = delimited_string({

        "  Building,",
        "    Small Office with AirflowNetwork model,  !- Name",
        "    0,                       !- North Axis {deg}",
        "    Suburbs,                 !- Terrain",
        "    0.001,                   !- Loads Convergence Tolerance Value",
        "    0.0050000,               !- Temperature Convergence Tolerance Value {deltaC}",
        "    FullInteriorAndExterior, !- Solar Distribution",
        "    25,                      !- Maximum Number of Warmup Days",
        "    6;                       !- Minimum Number of Warmup Days",

        "  Timestep,6;",

        "  SurfaceConvectionAlgorithm:Inside,TARP;",

        "  SurfaceConvectionAlgorithm:Outside,DOE-2;",

        "  HeatBalanceAlgorithm,ConductionTransferFunction;",

        "  ZoneCapacitanceMultiplier:ResearchSpecial,",
        "    Multiplier,              !- Name",
        "    ,                        !- Zone or ZoneList Name",
        "    1.0,                     !- Temperature Capacity Multiplier",
        "    1.0,                     !- Humidity Capacity Multiplier",
        "    1.0,                     !- Carbon Dioxide Capacity Multiplier",
        "    ;                        !- Generic Contaminant Capacity Multiplier",

        "  SimulationControl,",
        "    No,                     !- Do Zone Sizing Calculation",
        "    No,                     !- Do System Sizing Calculation",
        "    No,                      !- Do Plant Sizing Calculation",
        "    Yes,                     !- Run Simulation for Sizing Periods",
        "    No;                      !- Run Simulation for Weather File Run Periods",

        "  Sizing:Parameters,",
        "    1.53,                    !- Heating Sizing Factor",
        "    1.70,                    !- Cooling Sizing Factor",
        "    6;                       !- Timesteps in Averaging Window",

        "  RunPeriod,",
        "    Spring run,              !- Name",
        "    4,                       !- Begin Month",
        "    1,                       !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    4,                       !- End Month",
        "    1,                       !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  RunPeriod,",
        "    Winter run,              !- Name",
        "    1,                       !- Begin Month",
        "    14,                      !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    1,                       !- End Month",
        "    14,                      !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  RunPeriod,",
        "    Summer run,              !- Name",
        "    7,                       !- Begin Month",
        "    7,                       !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    7,                       !- End Month",
        "    7,                       !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  Site:Location,",
        "    CHICAGO_IL_USA TMY2-94846,  !- Name",
        "    41.78,                   !- Latitude {deg}",
        "    -87.75,                  !- Longitude {deg}",
        "    -6.00,                   !- Time Zone {hr}",
        "    190.00;                  !- Elevation {m}",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Heating 99% Design Conditions DB,  !- Name",
        "    1,                       !- Month",
        "    21,                      !- Day of Month",
        "    WinterDesignDay,         !- Day Type",
        "    -17.3,                   !- Maximum Dry-Bulb Temperature {C}",
        "    0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    -17.3,                   !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    4.9,                     !- Wind Speed {m/s}",
        "    270,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    0.0;                     !- Sky Clearness",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Cooling 1% Design Conditions DB/MCWB,  !- Name",
        "    7,                       !- Month",
        "    21,                      !- Day of Month",
        "    SummerDesignDay,         !- Day Type",
        "    31.5,                    !- Maximum Dry-Bulb Temperature {C}",
        "    10.7,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    23.0,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    5.3,                     !- Wind Speed {m/s}",
        "    230,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    1.0;                     !- Sky Clearness",

        "  Material,",
        "    A1 - 1 IN STUCCO,        !- Name",
        "    Smooth,                  !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    0.6918309,               !- Conductivity {W/m-K}",
        "    1858.142,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Material,",
        "    C4 - 4 IN COMMON BRICK,  !- Name",
        "    Rough,                   !- Roughness",
        "    0.1014984,               !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1922.216,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7600000,               !- Solar Absorptance",
        "    0.7600000;               !- Visible Absorptance",

        "  Material,",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Name",
        "    Smooth,                  !- Roughness",
        "    1.9050000E-02,           !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1601.846,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Material,",
        "    C6 - 8 IN CLAY TILE,     !- Name",
        "    Smooth,                  !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    0.5707605,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.8200000,               !- Solar Absorptance",
        "    0.8200000;               !- Visible Absorptance",

        "  Material,",
        "    C10 - 8 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "  Material,",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Name",
        "    Rough,                   !- Roughness",
        "    1.2710161E-02,           !- Thickness {m}",
        "    1.435549,                !- Conductivity {W/m-K}",
        "    881.0155,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5500000,               !- Solar Absorptance",
        "    0.5500000;               !- Visible Absorptance",

        "  Material,",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Name",
        "    Rough,                   !- Roughness",
        "    9.5402403E-03,           !- Thickness {m}",
        "    0.1902535,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "  Material,",
        "    B5 - 1 IN DENSE INSULATION,  !- Name",
        "    VeryRough,               !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    4.3239430E-02,           !- Conductivity {W/m-K}",
        "    91.30524,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5000000,               !- Solar Absorptance",
        "    0.5000000;               !- Visible Absorptance",

        "  Material,",
        "    C12 - 2 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    5.0901599E-02,           !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "  Material,",
        "    1.375in-Solid-Core,      !- Name",
        "    Smooth,                  !- Roughness",
        "    3.4925E-02,              !- Thickness {m}",
        "    0.1525000,               !- Conductivity {W/m-K}",
        "    614.5000,                !- Density {kg/m3}",
        "    1630.0000,               !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  WindowMaterial:Glazing,",
        "    ELECTRO GLASS LIGHT STATE,  !- Name",
        "    SpectralAverage,         !- Optical Data Type",
        "    ,                        !- Window Glass Spectral Data Set Name",
        "    0.006,                   !- Thickness {m}",
        "    0.814,                   !- Solar Transmittance at Normal Incidence",
        "    0.086,                   !- Front Side Solar Reflectance at Normal Incidence",
        "    0.086,                   !- Back Side Solar Reflectance at Normal Incidence",
        "    0.847,                   !- Visible Transmittance at Normal Incidence",
        "    0.099,                   !- Front Side Visible Reflectance at Normal Incidence",
        "    0.099,                   !- Back Side Visible Reflectance at Normal Incidence",
        "    0.0,                     !- Infrared Transmittance at Normal Incidence",
        "    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
        "    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
        "    0.9;                     !- Conductivity {W/m-K}",

        "  WindowMaterial:Glazing,",
        "    ELECTRO GLASS DARK STATE,!- Name",
        "    SpectralAverage,         !- Optical Data Type",
        "    ,                        !- Window Glass Spectral Data Set Name",
        "    0.006,                   !- Thickness {m}",
        "    0.111,                   !- Solar Transmittance at Normal Incidence",
        "    0.179,                   !- Front Side Solar Reflectance at Normal Incidence",
        "    0.179,                   !- Back Side Solar Reflectance at Normal Incidence",
        "    0.128,                   !- Visible Transmittance at Normal Incidence",
        "    0.081,                   !- Front Side Visible Reflectance at Normal Incidence",
        "    0.081,                   !- Back Side Visible Reflectance at Normal Incidence",
        "    0.0,                     !- Infrared Transmittance at Normal Incidence",
        "    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
        "    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
        "    0.9;                     !- Conductivity {W/m-K}",

        "  WindowMaterial:Gas,",
        "    WinAirGap,               !- Name",
        "    AIR,                     !- Gas Type",
        "    0.013;                   !- Thickness {m}",

        "  Construction,",
        "    EXTWALL80,               !- Name",
        "    A1 - 1 IN STUCCO,        !- Outside Layer",
        "    C4 - 4 IN COMMON BRICK,  !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "  Construction,",
        "    PARTITION06,             !- Name",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "    C6 - 8 IN CLAY TILE,     !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "  Construction,",
        "    FLOOR SLAB 8 IN,         !- Name",
        "    C10 - 8 IN HW CONCRETE;  !- Outside Layer",

        "  Construction,",
        "    ROOF34,                  !- Name",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Outside Layer",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Layer 2",
        "    C12 - 2 IN HW CONCRETE;  !- Layer 3",

        "  Construction,",
        "    CEILING:ZONE,            !- Name",
        "    B5 - 1 IN DENSE INSULATION,  !- Outside Layer",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 2",

        "  Construction,",
        "    CEILING:ATTIC,           !- Name",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "    B5 - 1 IN DENSE INSULATION;  !- Layer 2",

        "  Construction,",
        "    ELECTRO-CON-LIGHT,       !- Name",
        "    ELECTRO GLASS LIGHT STATE,  !- Outside Layer",
        "    WinAirGap,               !- Layer 2",
        "    ELECTRO GLASS LIGHT STATE;  !- Layer 3",

        "  Construction,",
        "    ELECTRO-CON-DARK,        !- Name",
        "    ELECTRO GLASS DARK STATE, !- Outside Layer",
        "    WinAirGap,               !- Layer 2",
        "    ELECTRO GLASS DARK STATE; !- Layer 3",

        "  Construction,",
        "    DOOR-CON,                !- Name",
        "    1.375in-Solid-Core;      !- Outside Layer",

        "  Zone,",
        "    West Zone,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    EAST ZONE,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    NORTH ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    ATTIC ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    ATTIC NORTH ZONE,        !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  GlobalGeometryRules,",
        "    UpperLeftCorner,         !- Starting Vertex Position",
        "    CounterClockWise,        !- Vertex Entry Direction",
        "    World;                   !- Coordinate System",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Flr002,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Wall003,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn005:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall008,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall009,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn005:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall010,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn005:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Roof001,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Roof002,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Flr002,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Wall009,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Wall010,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Roof001,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn001:Wall001:Win001,    !- Name",
        "    Window,                  !- Surface Type",
        "    ELECTRO-CON-LIGHT,       !- Construction Name",
        "    Zn001:Wall001,           !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,     !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    0.548000,0,2.5000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0.548000,0,0.5000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    5.548000,0,0.5000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    5.548000,0,2.5000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn001:Wall003:Door001,   !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Zn001:Wall003,           !- Building Surface Name",
        "    Zn003:Wall004:Door001,   !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    3.500,6.096000,2.0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    3.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    2.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    2.500,6.096000,2.0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn003:Wall002:Win001,    !- Name",
        "    Window,                  !- Surface Type",
        "    ELECTRO-CON-LIGHT,       !- Construction Name",
        "    Zn003:Wall002,           !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,     !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    5.548000,12.19200,2.5000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    5.548000,12.19200,0.5000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0.548000,12.19200,0.5000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0.548000,12.19200,2.5000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn003:Wall004:Door001,   !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Zn003:Wall004,           !- Building Surface Name",
        "    Zn001:Wall003:Door001,   !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    2.500,6.096000,2.0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    2.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    3.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    3.500,6.096000,2.0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  ScheduleTypeLimits,",
        "    Any Number;              !- Name",

        "  ScheduleTypeLimits,",
        "    Fraction,                !- Name",
        "    0.0,                     !- Lower Limit Value",
        "    1.0,                     !- Upper Limit Value",
        "    CONTINUOUS;              !- Numeric Type",

        "  ScheduleTypeLimits,",
        "    Temperature,             !- Name",
        "    -60,                     !- Lower Limit Value",
        "    200,                     !- Upper Limit Value",
        "    CONTINUOUS,              !- Numeric Type",
        "    Temperature;             !- Unit Type",

        "  ScheduleTypeLimits,",
        "    Control Type,            !- Name",
        "    0,                       !- Lower Limit Value",
        "    4,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "  ScheduleTypeLimits,",
        "    On/Off,                  !- Name",
        "    0,                       !- Lower Limit Value",
        "    1,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "  Schedule:Compact,",
        "    WindowVentSched,         !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,25.55,      !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: AllDays,            !- Field 6",
        "    Until: 24:00,21.11,      !- Field 7",
        "    Through: 12/31,          !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until: 24:00,25.55;      !- Field 11",

        "  Schedule:Compact,",
        "    Activity Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,131.8;      !- Field 3",

        "  Schedule:Compact,",
        "    Work Eff Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.0;        !- Field 3",

        "  Schedule:Compact,",
        "    Clothing Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,1.0;        !- Field 3",

        "  Schedule:Compact,",
        "    Air Velo Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.137;      !- Field 3",

        "  Schedule:Compact,",
        "    OFFICE OCCUPANCY,        !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,0.0,         !- Field 3",
        "    Until: 7:00,0.10,        !- Field 5",
        "    Until: 8:00,0.50,        !- Field 7",
        "    Until: 12:00,1.00,       !- Field 9",
        "    Until: 13:00,0.50,       !- Field 11",
        "    Until: 16:00,1.00,       !- Field 13",
        "    Until: 17:00,0.50,       !- Field 15",
        "    Until: 18:00,0.10,       !- Field 17",
        "    Until: 24:00,0.0,        !- Field 19",
        "    For: AllOtherDays,       !- Field 21",
        "    Until: 24:00,0.0;        !- Field 22",

        "  Schedule:Compact,",
        "    INTERMITTENT,            !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 8:00,0.0,         !- Field 3",
        "    Until: 18:00,1.00,       !- Field 5",
        "    Until: 24:00,0.0,        !- Field 7",
        "    For: AllOtherDays,       !- Field 9",
        "    Until: 24:00,0.0;        !- Field 10",

        "  Schedule:Compact,",
        "    OFFICE LIGHTING,         !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,0.05,        !- Field 3",
        "    Until: 7:00,0.20,        !- Field 5",
        "    Until: 17:00,1.00,       !- Field 7",
        "    Until: 18:00,0.50,       !- Field 9",
        "    Until: 24:00,0.05,       !- Field 11",
        "    For: AllOtherDays,       !- Field 13",
        "    Until: 24:00,0.05;       !- Field 14",

        "  Schedule:Compact,",
        "    FanAndCoilAvailSched,    !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,1.0,        !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: WeekDays SummerDesignDay, !- Field 6",
        "    Until: 7:00,0.0,         !- Field 7",
        "    Until: 17:00,1.0,        !- Field 9",
        "    Until: 24:00,0.0,        !- Field 11",
        "    For: WinterDesignDay,    !- Field 13",
        "    Until: 24:00,0.0,        !- Field 14",
        "    For: AllOtherDays,       !- Field 16",
        "    Until: 24:00,0.0,        !- Field 17",
        "    Through: 12/31,          !- Field 19",
        "    For: AllDays,            !- Field 20",
        "    Until: 24:00,1.0;        !- Field 21",

        "  Schedule:Compact,",
        "    CoolingCoilAvailSched,   !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.0,        !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: WeekDays SummerDesignDay, !- Field 6",
        "    Until: 7:00,0.0,         !- Field 7",
        "    Until: 17:00,1.0,        !- Field 9",
        "    Until: 24:00,0.0,        !- Field 11",
        "    For: WinterDesignDay,    !- Field 13",
        "    Until: 24:00,0.0,        !- Field 14",
        "    For: AllOtherDays,       !- Field 16",
        "    Until: 24:00,0.0,        !- Field 17",
        "    Through: 12/31,          !- Field 19",
        "    For: AllDays,            !- Field 20",
        "    Until: 24:00,0.0;        !- Field 21",

        "  Schedule:Compact,",
        "    Dual Heating Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,15.0,       !- Field 3",
        "    Until: 17:00,22.0,       !- Field 5",
        "    Until: 24:00,15.0;       !- Field 7",

        "  Schedule:Compact,",
        "    Dual Cooling Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,35.0,       !- Field 3",
        "    Until: 17:00,24.0,       !- Field 5",
        "    Until: 24:00,40.0;       !- Field 7",

        "  Schedule:Compact,",
        "    Dual Zone Control Type Sched,  !- Name",
        "    Control Type,            !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,4;          !- Field 3",

        "  Schedule:Compact,",
        "    VentingSched,            !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,1.0,        !- Field 3",
        "    Until: 17:00,0.0,        !- Field 5",
        "    Until: 24:00,1.0;        !- Field 7",

        "  Schedule:Compact,",
        "    Minimum OA Sch,          !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,0.10,       !- Field 3",
        "    Until: 17:00,0.25,       !- Field 5",
        "    Until: 24:00,0.10,       !- Field 7",
        "    Through: 9/30,           !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until: 24:00,0.25,       !- Field 11",
        "    Through: 12/31,          !- Field 13",
        "    For: AllDays,            !- Field 14",
        "    Until:  7:00,0.10,       !- Field 15",
        "    Until: 17:00,0.25,       !- Field 17",
        "    Until: 24:00,0.10;       !- Field 19",

        "  People,",
        "    West Zone People,        !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    OFFICE OCCUPANCY,        !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    3.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "  People,",
        "    EAST ZONE People,        !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    OFFICE OCCUPANCY,        !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    3.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "  People,",
        "    NORTH ZONE People,       !- Name",
        "    NORTH ZONE,              !- Zone or ZoneList Name",
        "    OFFICE OCCUPANCY,        !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    4.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "  Lights,",
        "    West Zone Lights 1,      !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    1.0,                     !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "  Lights,",
        "    EAST ZONE Lights 1,      !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    1.0,                     !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "  Lights,",
        "    NORTH ZONE Lights 1,     !- Name",
        "    NORTH ZONE,              !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    1.0,                     !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "  ElectricEquipment,",
        "    West Zone ElecEq 1,      !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    2928.751,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "  ElectricEquipment,",
        "    EAST ZONE ElecEq 1,      !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    1464.375,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "  ElectricEquipment,",
        "    NORTH ZONE ElecEq 1,     !- Name",
        "    NORTH ZONE,              !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    2928.751,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "  Daylighting:Controls,",
        "    West Zone_DaylCtrl,      !- Name",
        "    West Zone,               !- Zone Name",
        "    SplitFlux,               !- Daylighting Method",
        "    ,                        !- Availability Schedule Name",
        "    Continuous,              !- Lighting Control Type",
        "    0.3,                     !- Minimum Input Power Fraction for Continuous or ContinuousOff Dimming Control",
        "    0.2,                     !- Minimum Light Output Fraction for Continuous or ContinuousOff Dimming Control",
        "    ,                        !- Number of Stepped Control Steps",
        "    1.0,                     !- Probability Lighting will be Reset When Needed in Manual Stepped Control",
        "    West Zone_DaylRefPt1,    !- Glare Calculation Daylighting Reference Point Name",
        "    180.0,                   !- Glare Calculation Azimuth Angle of View Direction Clockwise from Zone y-Axis {deg}",
        "    20.0,                    !- Maximum Allowable Discomfort Glare Index",
        "    ,                        !- DElight Gridding Resolution {m2}",
        "    West Zone_DaylRefPt1,    !- Daylighting Reference Point 1 Name",
        "    1.0,                     !- Fraction of Zone Controlled by Reference Point 1",
        "    500.;                    !- Illuminance Setpoint at Reference Point 1 {lux}",

        "  Daylighting:ReferencePoint,",
        "    West Zone_DaylRefPt1,    !- Name",
        "    West Zone,               !- Zone Name",
        "    3.048,                   !- X-Coordinate of Reference Point {m}",
        "    3.048,                   !- Y-Coordinate of Reference Point {m}",
        "    0.9;                     !- Z-Coordinate of Reference Point {m}",

        "  Curve:Biquadratic,",
        "    WindACCoolCapFT,         !- Name",
        "    0.942587793,             !- Coefficient1 Constant",
        "    0.009543347,             !- Coefficient2 x",
        "    0.000683770,             !- Coefficient3 x**2",
        "    -0.011042676,            !- Coefficient4 y",
        "    0.000005249,             !- Coefficient5 y**2",
        "    -0.000009720,            !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111,                !- Maximum Value of y",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Temperature,             !- Input Unit Type for Y",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Biquadratic,",
        "    WindACEIRFT,             !- Name",
        "    0.342414409,             !- Coefficient1 Constant",
        "    0.034885008,             !- Coefficient2 x",
        "    -0.000623700,            !- Coefficient3 x**2",
        "    0.004977216,             !- Coefficient4 y",
        "    0.000437951,             !- Coefficient5 y**2",
        "    -0.000728028,            !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111,                !- Maximum Value of y",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Temperature,             !- Input Unit Type for Y",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Quadratic,",
        "    WindACCoolCapFFF,        !- Name",
        "    0.8,                     !- Coefficient1 Constant",
        "    0.2,                     !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "  Curve:Quadratic,",
        "    WindACEIRFFF,            !- Name",
        "    1.1552,                  !- Coefficient1 Constant",
        "    -0.1808,                 !- Coefficient2 x",
        "    0.0256,                  !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "  Curve:Quadratic,",
        "    WindACPLFFPLR,           !- Name",
        "    0.85,                    !- Coefficient1 Constant",
        "    0.15,                    !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.0,                     !- Minimum Value of x",
        "    1.0;                     !- Maximum Value of x",

        "  NodeList,",
        "    OutsideAirInletNodes,    !- Name",
        "    Outside Air Inlet Node;  !- Node 1 Name",

        "  NodeList,",
        "    Zone1Inlets,             !- Name",
        "    Zone 1 NoReheat Air Outlet Node;  !- Node 1 Name",

        "  NodeList,",
        "    Zone2Inlets,             !- Name",
        "    Zone 2 Reheat Air Outlet Node;  !- Node 1 Name",

        "  NodeList,",
        "    Supply Air Temp Nodes,   !- Name",
        "    Heating Coil Air Inlet Node,  !- Node 1 Name",
        "    Air Loop Outlet Node;    !- Node 2 Name",

        "  BranchList,",
        "    Air Loop Branches,       !- Name",
        "    Air Loop Main Branch;    !- Branch 1 Name",

        "  Branch,",
        "    Air Loop Main Branch,    !- Name",
        "    ,                        !- Pressure Drop Curve Name",
        "    AirLoopHVAC:OutdoorAirSystem,  !- Component 1 Object Type",
        "    OA Sys 1,                !- Component 1 Name",
        "    Air Loop Inlet Node,     !- Component 1 Inlet Node Name",
        "    Mixed Air Node,          !- Component 1 Outlet Node Name",
        "    Fan:ConstantVolume,      !- Component 2 Object Type",
        "    Supply Fan 1,            !- Component 2 Name",
        "    Mixed Air Node,          !- Component 2 Inlet Node Name",
        "    Cooling Coil Air Inlet Node,  !- Component 2 Outlet Node Name",
        "    CoilSystem:Cooling:DX,   !- Component 3 Object Type",
        "    DX Cooling Coil System 1,!- Component 3 Name",
        "    Cooling Coil Air Inlet Node,  !- Component 3 Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- Component 3 Outlet Node Name",
        "    Coil:Heating:Fuel,       !- Component 4 Object Type",
        "    Main Heating Coil 1,     !- Component 4 Name",
        "    Heating Coil Air Inlet Node,  !- Component 4 Inlet Node Name",
        "    Air Loop Outlet Node;    !- Component 4 Outlet Node Name",

        "  AirLoopHVAC,",
        "    Typical Terminal Reheat 1,  !- Name",
        "    ,                        !- Controller List Name",
        "    Reheat System 1 Avail List,  !- Availability Manager List Name",
        "    1.16,                     !- Design Supply Air Flow Rate {m3/s}",
        "    Air Loop Branches,       !- Branch List Name",
        "    ,                        !- Connector List Name",
        "    Air Loop Inlet Node,     !- Supply Side Inlet Node Name",
        "    Return Air Mixer Outlet, !- Demand Side Outlet Node Name",
        "    Zone Equipment Inlet Node,  !- Demand Side Inlet Node Names",
        "    Air Loop Outlet Node;    !- Supply Side Outlet Node Names",

        "  AirLoopHVAC:ControllerList,",
        "    OA Sys 1 Controllers,    !- Name",
        "    Controller:OutdoorAir,   !- Controller 1 Object Type",
        "    OA Controller 1;         !- Controller 1 Name",

        "  AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "    OA Sys 1 Equipment,      !- Name",
        "    OutdoorAir:Mixer,        !- Component 1 Object Type",
        "    OA Mixing Box 1;         !- Component 1 Name",

        "  AirLoopHVAC:OutdoorAirSystem,",
        "    OA Sys 1,                !- Name",
        "    OA Sys 1 Controllers,    !- Controller List Name",
        "    OA Sys 1 Equipment;      !- Outdoor Air Equipment List Name",

        "  OutdoorAir:NodeList,",
        "    OutsideAirInletNodes;    !- Node or NodeList Name 1",

        "  OutdoorAir:Mixer,",
        "    OA Mixing Box 1,         !- Name",
        "    Mixed Air Node,          !- Mixed Air Node Name",
        "    Outside Air Inlet Node,  !- Outdoor Air Stream Node Name",
        "    Relief Air Outlet Node,  !- Relief Air Stream Node Name",
        "    Air Loop Inlet Node;     !- Return Air Stream Node Name",

        "  AirflowNetwork:SimulationControl,",
        "    AirflowNetwork_All,      !- Name",
        "    MultizoneWithDistribution,  !- AirflowNetwork Control",
        "    INPUT,                   !- Wind Pressure Coefficient Type",
        "    ExternalNode,            !- Height Selection for Local Wind Pressure Calculation",
        "    LOWRISE,                 !- Building Type",
        "    500,                     !- Maximum Number of Iterations {dimensionless}",
        "    ZeroNodePressures,       !- Initialization Type",
        "    1.0E-04,                 !- Relative Airflow Convergence Tolerance {dimensionless}",
        "    1.0E-04,                 !- Absolute Airflow Convergence Tolerance {kg/s}",
        "    -0.5,                    !- Convergence Acceleration Limit {dimensionless}",
        "    0.0,                     !- Azimuth Angle of Long Axis of Building {deg}",
        "    1.0;                     !- Ratio of Building Width Along Short Axis to Width Along Long Axis",

        "  AirflowNetwork:MultiZone:Zone,",
        "    West Zone,               !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    0.3,                     !- Minimum Venting Open Factor {dimensionless}",
        "    5.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    10.0,                    !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0,                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",
        "    VentingSched;            !- Venting Availability Schedule Name",

        "  AirflowNetwork:MultiZone:Zone,",
        "    EAST ZONE,               !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    NORTH ZONE,              !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0,                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",
        "    VentingSched;            !- Venting Availability Schedule Name",

        "  AirflowNetwork:MultiZone:Zone,",
        "    ATTIC ZONE,              !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall001,           !- Surface Name",
        "    ELA-1,                   !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall001:Win001,    !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall003,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall003:Door001,   !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall004,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall003,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    EFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall005,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall002:Win001,    !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall003,           !- Surface Name",
        "    NORTH ZONE Exhaust Fan,       !- Leakage Component Name",
        "    EFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn004:Roof001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn004:Roof002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn005:Roof001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn005:Wall004,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    ,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn005:Wall005,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    ,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Component:DetailedOpening,",
        "    WiOpen1,                 !- Name",
        "    0.0001,                  !- Air Mass Flow Coefficient When Opening is Closed {kg/s-m}",
        "    0.667,                   !- Air Mass Flow Exponent When Opening is Closed {dimensionless}",
        "    NonPivoted,              !- Type of Rectangular Large Vertical Opening (LVO)",
        "    0.0,                     !- Extra Crack Length or Height of Pivoting Axis {m}",
        "    2,                       !- Number of Sets of Opening Factor Data",
        "    0.0,                     !- Opening Factor 1 {dimensionless}",
        "    0.5,                     !- Discharge Coefficient for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Width Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Opening Factor 2 {dimensionless}",
        "    0.6,                     !- Discharge Coefficient for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Width Factor for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 2 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 2 {dimensionless}",
        "    0,                       !- Opening Factor 3 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 3 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Start Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Opening Factor 4 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 4 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 4 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 4 {dimensionless}",
        "    0;                       !- Start Height Factor for Opening Factor 4 {dimensionless}",

        "  AirflowNetwork:MultiZone:Component:SimpleOpening,",
        "    DrOpen,                  !- Name",
        "    0.0001,                  !- Air Mass Flow Coefficient When Opening is Closed {kg/s-m}",
        "    0.667,                   !- Air Mass Flow Exponent When Opening is Closed {dimensionless}",
        "    0.0001,                  !- Minimum Density Difference for Two-Way Flow {kg/m3}",
        "    0.55;                    !- Discharge Coefficient {dimensionless}",

        "  AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "    ReferenceCrackConditions,!- Name",
        "    20.0,                    !- Reference Temperature {C}",
        "    101325,                  !- Reference Barometric Pressure {Pa}",
        "    0.0;                     !- Reference Humidity Ratio {kgWater/kgDryAir}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CR-1,                    !- Name",
        "    0.001,                   !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CRcri,                   !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Component:ZoneExhaustFan,",
        "    NORTH ZONE Exhaust Fan,       !- Name",
        "    0.01,                    !- Air Mass Flow Coefficient When the Zone Exhaust Fan is Off at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When the Zone Exhaust Fan is Off {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:EffectiveLeakageArea,",
        "    ELA-1,                   !- Name",
        "    0.007,                   !- Effective Leakage Area {m2}",
        "    1.0,                     !- Discharge Coefficient {dimensionless}",
        "    4.0,                     !- Reference Pressure Difference {Pa}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    NFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    NFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    EFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    EFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    SFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    SFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    WFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    WFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    Horizontal,              !- Name",
        "    3.028,                   !- External Node Height {m}",
        "    Horizontal_WPCValue;     !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientArray,",
        "    Every 30 Degrees,        !- Name",
        "    0,                       !- Wind Direction 1 {deg}",
        "    30,                      !- Wind Direction 2 {deg}",
        "    60,                      !- Wind Direction 3 {deg}",
        "    90,                      !- Wind Direction 4 {deg}",
        "    120,                     !- Wind Direction 5 {deg}",
        "    150,                     !- Wind Direction 6 {deg}",
        "    180,                     !- Wind Direction 7 {deg}",
        "    210,                     !- Wind Direction 8 {deg}",
        "    240,                     !- Wind Direction 9 {deg}",
        "    270,                     !- Wind Direction 10 {deg}",
        "    300,                     !- Wind Direction 11 {deg}",
        "    330;                     !- Wind Direction 12 {deg}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    NFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.60,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.48;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    EFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.56;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    SFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.37,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.42;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    WFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.04;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    Horizontal_WPCValue,     !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.00,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.00;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentInletNode,      !- Name",
        "    Zone Equipment Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentOutletNode,     !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    SupplyMainNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainSplitterNode,        !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1SupplyRegisterNode, !- Name",
        "    Zone 1 NoReheat Air Outlet Node,       !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1OutletNode,         !- Name",
        "    Zone 1 Outlet Node,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ReheatInlet2Node,        !- Name",
        "    Zone 2 Reheat Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2SupplyRegisterNode, !- Name",
        "    Zone 2 Reheat Air Outlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2OutletNode,         !- Name",
        "    Zone 2 Outlet Node,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainMixerNode,           !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainReturnNode,          !- Name",
        "    Return Air Mixer Outlet, !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainInletNode,           !- Name",
        "    Air Loop Inlet Node,     !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    OA System Node,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:OutdoorAirSystem,  !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    OA Inlet Node,           !- Name",
        "    Outside Air Inlet Node,  !- Component Name or Node Name",
        "    OAMixerOutdoorAirStreamNode,  !- Component Object Type or Node Type",
        "    1.5;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanInletNode,            !- Name",
        "    Mixed Air Node,          !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanOutletNode,           !- Name",
        "    Cooling Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingInletNode,        !- Name",
        "    Heating Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingOutletNode,       !- Name",
        "    Air Loop Outlet Node,    !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Component:Leak,",
        "    MainSupplyLeak,          !- Name",
        "    0.0025,                  !- Air Mass Flow Coefficient {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:ConstantPressureDrop,",
        "    SupplyCPDComp,           !- Name",
        "    1.0;                     !- Pressure Difference Across the Component {Pa}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ZoneSupplyELR1,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ZoneSupplyELR2,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeakELR1,          !- Name",
        "    0.03,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    41.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeakELR2,          !- Name",
        "    0.03,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    40.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTruck1,              !- Name",
        "    3.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTruck2,              !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone1Supply,             !- Name",
        "    5.0,                     !- Duct Length {m}",
        "    0.4,                     !- Hydraulic Diameter {m}",
        "    0.1256,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone2Supply,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.39,                    !- Hydraulic Diameter {m}",
        "    0.1195,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    2.5,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone1Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone2Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.48,                    !- Hydraulic Diameter {m}",
        "    0.1809,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneConnectionDuct,      !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    30.00,                   !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MixerConnectionDuct,     !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopReturn,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopSupply,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Fan,",
        "    Supply Fan 1,            !- Fan Name",
        "    Fan:ConstantVolume;      !- Supply Fan Object Type",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    ACDXCoil 1,              !- Coil Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Main Heating Coil 1,     !- Coil Name",
        "    Coil:Heating:Fuel,       !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:TerminalUnit,",
        "    Reheat Zone 2,           !- Terminal Unit Name",
        "    AirTerminal:SingleDuct:ConstantVolume:Reheat,  !- Terminal Unit Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    0.44;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link 1,             !- Name",
        "    EquipmentInletNode,      !- Node 1 Name",
        "    EquipmentOutletNode,     !- Node 2 Name",
        "    MainTruck1,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main CDP Link,           !- Name",
        "    EquipmentOutletNode,     !- Node 1 Name",
        "    SupplyMainNode,          !- Node 2 Name",
        "    SupplyCPDComp;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link 2,             !- Name",
        "    SupplyMainNode,          !- Node 1 Name",
        "    MainSplitterNode,        !- Node 2 Name",
        "    MainTruck2,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Supply1Link,        !- Name",
        "    MainSplitterNode,        !- Node 1 Name",
        "    Zone1SupplyNode,         !- Node 2 Name",
        "    Zone1Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Supply2Link,        !- Name",
        "    Zone1SupplyNode,         !- Node 1 Name",
        "    Zone1SupplyRegisterNode,        !- Node 2 Name",
        "    Zone1Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1SupplyConnectionLink,  !- Name",
        "    Zone1SupplyRegisterNode, !- Node 1 Name",
        "    West Zone,               !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1ReturnConnectionLink,  !- Name",
        "    West Zone,               !- Node 1 Name",
        "    Zone1OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Supply1Link,        !- Name",
        "    MainSplitterNode,        !- Node 1 Name",
        "    Zone2SupplyNode,         !- Node 2 Name",
        "    Zone2Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Supply2Link,        !- Name",
        "    Zone2SupplyNode,         !- Node 1 Name",
        "    ReheatInlet2Node,        !- Node 2 Name",
        "    Zone2Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2ReheatCoilLink,     !- Name",
        "    ReheatInlet2Node,        !- Node 1 Name",
        "    Zone2SupplyRegisterNode, !- Node 2 Name",
        "    Reheat Zone 2;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2SupplyConnectionLink,  !- Name",
        "    Zone2SupplyRegisterNode, !- Node 1 Name",
        "    EAST ZONE,               !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2returnConnectionLink,  !- Name",
        "    EAST ZONE,               !- Node 1 Name",
        "    Zone2OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Return1Link,        !- Name",
        "    Zone1OutletNode,         !- Node 1 Name",
        "    Zone1ReturnNode,         !- Node 2 Name",
        "    Zone1Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Return2Link,        !- Name",
        "    Zone1ReturnNode,         !- Node 1 Name",
        "    MainMixerNode,           !- Node 2 Name",
        "    Zone1Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Return1Link,        !- Name",
        "    Zone2OutletNode,         !- Node 1 Name",
        "    Zone2ReturnNode,         !- Node 2 Name",
        "    Zone2Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Return2Link,        !- Name",
        "    Zone2ReturnNode,         !- Node 1 Name",
        "    MainMixerNode,           !- Node 2 Name",
        "    Zone2Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ReturnMixerLink,         !- Name",
        "    MainMixerNode,           !- Node 1 Name",
        "    MainReturnNode,          !- Node 2 Name",
        "    MixerConnectionDuct,     !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    AirLoopReturnLink,       !- Name",
        "    MainReturnNode,          !- Node 1 Name",
        "    MainInletNode,           !- Node 2 Name",
        "    AirLoopReturn;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemInletLink,       !- Name",
        "    MainInletNode,           !- Node 1 Name",
        "    OA System Node,          !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OAMixerOutletLink,       !- Name",
        "    OA System Node,          !- Node 1 Name",
        "    FanInletNode,            !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SupplyFanLink,           !- Name",
        "    FanInletNode,            !- Node 1 Name",
        "    FanOutletNode,           !- Node 2 Name",
        "    Supply Fan 1;            !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    CoolingCoilLink,         !- Name",
        "    FanOutletNode,           !- Node 1 Name",
        "    HeatingInletNode,        !- Node 2 Name",
        "    ACDXCoil 1;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    HeatingCoilLink,         !- Name",
        "    HeatingInletNode,        !- Node 1 Name",
        "    HeatingOutletNode,       !- Node 2 Name",
        "    Main Heating Coil 1;     !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    EquipmentAirLoopLink,    !- Name",
        "    HeatingOutletNode,       !- Node 1 Name",
        "    EquipmentInletNode,      !- Node 2 Name",
        "    AirLoopSupply;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1ReturnLeakLink,     !- Name",
        "    Zone1ReturnNode,         !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    ReturnLeakELR1;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    MainSupplyLeakLink,      !- Name",
        "    SupplyMainNode,          !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    MainSupplyLeak;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1SupplyLeakLink,     !- Name",
        "    Zone1SupplyNode,         !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    ZoneSupplyELR1;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2ReturnLeakLink,     !- Name",
        "    Zone2ReturnNode,         !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    ReturnLeakELR2;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2SupplyLeakLink,     !- Name",
        "    Zone2SupplyNode,         !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    ZoneSupplyELR2;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemFanLink,       !- Name",
        "    OA Inlet Node,           !- Node 1 Name",
        "    OA System Node,          !- Node 2 Name",
        "    OA Fan;                  !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemReliefLink,      !- Name",
        "    OA System Node,          !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    Relief Fan;              !- Component Name",

        "  AirflowNetwork:Distribution:Component:OutdoorAirFlow,",
        "    OA Fan,                  !- Name",
        "    OA Mixing Box 1,         !- Outdoor Air Mixer Name",
        "    0.001,                   !- Air Mass Flow Coefficient When No Outdoor Air Flow at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When No Outdoor Air Flow {dimensionless}",

        "  AirflowNetwork:Distribution:Component:ReliefAirFlow,",
        "    Relief Fan,              !- Name",
        "    OA Mixing Box 1,         !- Outdoor Air Mixer Name",
        "    0.001,                   !- Air Mass Flow Coefficient When No Outdoor Air Flow at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When No Outdoor Air Flow {dimensionless}",

        "  Schedule:Compact,",
        "    Pressure Setpoint Schedule,  !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,5.0,        !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: AllDays,            !- Field 6",
        "    Until: 24:00,9.5,        !- Field 7",
        "    Through: 12/31,          !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until: 24:00,5.0;        !- Field 11",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 EquipmentInletNode,      !- Name",
        "    Heat Pump 1 Supply Path Inlet,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 EquipmentOutletNode,     !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 SupplyMainNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 MainSplitterNode,        !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 Zone1SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 Zone1SupplyRegisterNode, !- Name",
        "    NORTH ZONE NoReheat Air Outlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 Zone1OutletNode,         !- Name",
        "    NORTH ZONE Return Outlet,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 Zone1ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 MainMixerNode,           !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 MainReturnNode,          !- Name",
        "    Heat Pump 1 Return Air Outlet, !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 MainInletNode,           !- Name",
        "    Heat Pump 1 Air Loop Inlet,     !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 OA System Node,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:OutdoorAirSystem,  !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 OA Inlet Node,           !- Name",
        "    Heat Pump 1 Outside Air Inlet,  !- Component Name or Node Name",
        "    OAMixerOutdoorAirStreamNode,  !- Component Object Type or Node Type",
        "    1.5;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 FanInletNode,            !- Name",
        "    Heat Pump 1 Mixed Air Outlet,          !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 FanOutletNode,           !- Name",
        "    Heat Pump 1 Supply Fan Outlet,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 HeatingInletNode,        !- Name",
        "    Heat Pump 1 Cooling Coil Outlet,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 HeatingOutletNode,       !- Name",
        "    Heat Pump 1 Heating Coil Outlet,    !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Heat Pump 1 SuppHeatingOutletNode,       !- Name",
        "    Heat Pump 1 Air Loop Outlet,    !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",
        "  AirflowNetwork:Distribution:Component:Leak,",
        "    Heat Pump 1 MainSupplyLeak,          !- Name",
        "    0.0025,                  !- Air Mass Flow Coefficient {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    Heat Pump 1 ZoneSupplyELR1,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    Heat Pump 1 ReturnLeakELR1,          !- Name",
        "    0.03,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    41.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Heat Pump 1 MainTruck1,              !- Name",
        "    3.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    5.0,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Heat Pump 1 MainTruck2,              !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Heat Pump 1 Zone1Supply,             !- Name",
        "    5.0,                     !- Duct Length {m}",
        "    0.4,                     !- Hydraulic Diameter {m}",
        "    0.1256,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Heat Pump 1 Zone1Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Fan,",
        "    Heat Pump 1 Supply Fan,            !- Fan Name",
        "    Fan:OnOff;      !- Supply Fan Object Type",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Heat Pump 1 Cooling Coil,              !- Coil Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Heat Pump 1 HP Heating Coil,     !- Coil Name",
        "    Coil:Heating:DX:SingleSpeed,       !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Heat Pump 1 Sup Heat Coil,     !- Coil Name",
        "    Coil:Heating:Electric,       !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Main Link 1,             !- Name",
        "    Heat Pump 1 EquipmentInletNode,      !- Node 1 Name",
        "    Heat Pump 1 EquipmentOutletNode,     !- Node 2 Name",
        "    Heat Pump 1 MainTruck1,              !- Component Name",
        "    Attic NORTH ZONE;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Main Link 2,             !- Name",
        "    Heat Pump 1 EquipmentOutletNode,          !- Node 1 Name",
        "    Heat Pump 1 SupplyMainNode,        !- Node 2 Name",
        "    Heat Pump 1 MainTruck2,              !- Component Name",
        "    Attic NORTH ZONE;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1Supply1Link,        !- Name",
        "    Heat Pump 1 SupplyMainNode,        !- Node 1 Name",
        "    Heat Pump 1 MainSplitterNode,         !- Node 2 Name",
        "    Heat Pump 1 Zone1Supply,             !- Component Name",
        "    Attic NORTH ZONE;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1Supply2Link,        !- Name",
        "    Heat Pump 1 MainSplitterNode,         !- Node 1 Name",
        "    Heat Pump 1 Zone1SupplyNode,        !- Node 2 Name",
        "    Heat Pump 1 Zone1Supply,             !- Component Name",
        "    Attic NORTH Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1InletlLink,     !- Name",
        "    Heat Pump 1 Zone1SupplyNode,        !- Node 1 Name",
        "    Heat Pump 1 Zone1SupplyRegisterNode, !- Node 2 Name",
        "    Heat Pump 1 Zone1Supply,             !- Component Name",
        "    Attic NORTH Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1SupplyConnectionLink,  !- Name",
        "    Heat Pump 1 Zone1SupplyRegisterNode, !- Node 1 Name",
        "    NORTH Zone,               !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1ReturnConnectionLink,  !- Name",
        "    NORTH Zone,               !- Node 1 Name",
        "    Heat Pump 1 Zone1OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1Return1Link,        !- Name",
        "    Heat Pump 1 Zone1OutletNode,         !- Node 1 Name",
        "    Heat Pump 1 Zone1ReturnNode,         !- Node 2 Name",
        "    Heat Pump 1 Zone1Return,             !- Component Name",
        "    Attic NORTH Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1Return2Link,        !- Name",
        "    Heat Pump 1 Zone1ReturnNode,         !- Node 1 Name",
        "    Heat Pump 1 MainMixerNode,           !- Node 2 Name",
        "    Heat Pump 1 Zone1Return,             !- Component Name",
        "    Attic NORTH Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 ReturnMixerLink,         !- Name",
        "    Heat Pump 1 MainMixerNode,           !- Node 1 Name",
        "    Heat Pump 1 MainReturnNode,          !- Node 2 Name",
        "    MixerConnectionDuct,     !- Component Name",
        "    Attic NORTH Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 AirLoopReturnLink,       !- Name",
        "    Heat Pump 1 MainReturnNode,          !- Node 1 Name",
        "    Heat Pump 1 MainInletNode,           !- Node 2 Name",
        "    AirLoopReturn;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 OASystemInletLink,       !- Name",
        "    Heat Pump 1 MainInletNode,           !- Node 1 Name",
        "    Heat Pump 1 OA System Node,          !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 OAMixerOutletLink,       !- Name",
        "    Heat Pump 1 OA System Node,          !- Node 1 Name",
        "    Heat Pump 1 FanInletNode,            !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 OASystemFanLink,       !- Name",
        "    Heat Pump 1 OA Inlet Node,           !- Node 1 Name",
        "    Heat Pump 1 OA System Node,          !- Node 2 Name",
        "    Heat Pump 1 OA Fan;                  !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 SupplyFanLink,           !- Name",
        "    Heat Pump 1 FanInletNode,            !- Node 1 Name",
        "    Heat Pump 1 FanOutletNode,           !- Node 2 Name",
        "    Heat Pump 1 Supply Fan;            !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 CoolingCoilLink,         !- Name",
        "    Heat Pump 1 FanOutletNode,           !- Node 1 Name",
        "    Heat Pump 1 HeatingInletNode,        !- Node 2 Name",
        "    Heat Pump 1 Cooling Coil;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 HeatingCoilLink,         !- Name",
        "    Heat Pump 1 HeatingInletNode,        !- Node 1 Name",
        "    Heat Pump 1 HeatingOutletNode,       !- Node 2 Name",
        "    Heat Pump 1 HP Heating Coil;     !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 SuppHeatingCoilLink,         !- Name",
        "    Heat Pump 1 HeatingOutletNode,        !- Node 1 Name",
        "    Heat Pump 1 SuppHeatingOutletNode,       !- Node 2 Name",
        "    Heat Pump 1 Sup Heat Coil;     !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 EquipmentAirLoopLink,    !- Name",
        "    Heat Pump 1 SuppHeatingOutletNode,       !- Node 1 Name",
        "    Heat Pump 1 EquipmentInletNode,      !- Node 2 Name",
        "    AirLoopSupply;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 Zone1ReturnLeakLink,     !- Name",
        "    Heat Pump 1 Zone1ReturnNode,         !- Node 1 Name",
        "    Heat Pump 1 OA Inlet Node,           !- Node 2 Name",
        "    Heat Pump 1 ReturnLeakELR1;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Heat Pump 1 MainSupplyLeakLink,      !- Name",
        "    Heat Pump 1 SupplyMainNode,          !- Node 1 Name",
        "    ATTIC NORTH ZONE,              !- Node 2 Name",
        "    Heat Pump 1 MainSupplyLeak;          !- Component Name",
        "  AirflowNetwork:Distribution:Component:OutdoorAirFlow,",
        "    Heat Pump 1 OA Fan,                  !- Name",
        "    Heat Pump 1 OA Mixing Box,         !- Outdoor Air Mixer Name",
        "    0.001,                   !- Air Mass Flow Coefficient When No Outdoor Air Flow at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When No Outdoor Air Flow {dimensionless}",

        "  AvailabilityManagerAssignmentList,",
        "    Reheat System 1 Avail List,  !- Name",
        "    AvailabilityManager:Scheduled,  !- Availability Manager 1 Object Type",
        "    Reheat System 1 Avail;   !- Availability Manager 1 Name",

        "  AvailabilityManager:Scheduled,",
        "    Reheat System 1 Avail,   !- Name",
        "    FanAndCoilAvailSched;    !- Schedule Name",

        "  SetpointManager:SingleZone:Reheat,",
        "    Supply Air Temp Manager, !- Name",
        "    Temperature,             !- Control Variable",
        "    13.,                     !- Minimum Supply Air Temperature {C}",
        "    45.,                     !- Maximum Supply Air Temperature {C}",
        "    WEST ZONE,              !- Control Zone Name",
        "    Zone 1 Node,             !- Zone Node Name",
        "    Zone 1 NoReheat Air Outlet Node,       !- Zone Inlet Node Name",
        "    Supply Air Temp Nodes;   !- Setpoint Node or NodeList Name",

        "  Controller:OutdoorAir,",
        "    OA Controller 1,         !- Name",
        "    Relief Air Outlet Node,  !- Relief Air Outlet Node Name",
        "    Air Loop Inlet Node,     !- Return Air Node Name",
        "    Mixed Air Node,          !- Mixed Air Node Name",
        "    Outside Air Inlet Node,  !- Actuator Node Name",
        "    0.2333,                  !- Minimum Outdoor Air Flow Rate {m3/s}",
        "    1.16,                     !- Maximum Outdoor Air Flow Rate {m3/s}",
        "    NoEconomizer,            !- Economizer Control Type",
        "    ModulateFlow,            !- Economizer Control Action Type",
        "    19.,                     !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "    ,                        !- Economizer Maximum Limit Enthalpy {J/kg}",
        "    ,                        !- Economizer Maximum Limit Dewpoint Temperature {C}",
        "    ,                        !- Electronic Enthalpy Limit Curve Name",
        "    4.,                      !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "    NoLockout,               !- Lockout Type",
        "    FixedMinimum;            !- Minimum Limit Type",

        "  ZoneHVAC:EquipmentConnections,",
        "    West Zone,               !- Zone Name",
        "    Zone1Equipment,          !- Zone Conditioning Equipment List Name",
        "    Zone1Inlets,             !- Zone Air Inlet Node or NodeList Name",
        "    ,                        !- Zone Air Exhaust Node or NodeList Name",
        "    Zone 1 Node,             !- Zone Air Node Name",
        "    Zone 1 Outlet Node;      !- Zone Return Air Node or NodeList Name",

        "  ZoneHVAC:EquipmentConnections,",
        "    EAST ZONE,               !- Zone Name",
        "    Zone2Equipment,          !- Zone Conditioning Equipment List Name",
        "    Zone2Inlets,             !- Zone Air Inlet Node or NodeList Name",
        "    ,                        !- Zone Air Exhaust Node or NodeList Name",
        "    Zone 2 Node,             !- Zone Air Node Name",
        "    Zone 2 Outlet Node;      !- Zone Return Air Node or NodeList Name",

        "  ZoneHVAC:EquipmentList,",
        "    Zone1Equipment,          !- Name",
        "    SequentialLoad,          !- Load Distribution Scheme",
        "    ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "    Zone1NoReheat,           !- Zone Equipment 1 Name",
        "    1,                       !- Zone Equipment 1 Cooling Sequence",
        "    1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "    ,                        !- Zone Equipment 1 Sequential Cooling Fraction",
        "    ;                        !- Zone Equipment 1 Sequential Heating Fraction",

        "  ZoneHVAC:EquipmentList,",
        "    Zone2Equipment,          !- Name",
        "    SequentialLoad,          !- Load Distribution Scheme",
        "    ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "    Zone2TermReheat,         !- Zone Equipment 1 Name",
        "    1,                       !- Zone Equipment 1 Cooling Sequence",
        "    1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "    ,                        !- Zone Equipment 1 Sequential Cooling Fraction",
        "    ;                        !- Zone Equipment 1 Sequential Heating Fraction",

        "  ZoneHVAC:AirDistributionUnit,",
        "    Zone1NoReheat,         !- Name",
        "    Zone 1 NoReheat Air Outlet Node,  !- Air Distribution Unit Outlet Node Name",
        "    AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "    No Reheat Zone 1;           !- Air Terminal Name",

        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "    No Reheat Zone 1,           !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    Zone 1 NoReheat Air Inlet Node,  !- Air Inlet Node Name",
        "    Zone 1 NoReheat Air Outlet Node,  !- Air Outlet Node Name",
        "    0.64;                    !- Maximum Air Flow Rate {m3/s}",

        "  ZoneHVAC:AirDistributionUnit,",
        "    Zone2TermReheat,         !- Name",
        "    Zone 2 Reheat Air Outlet Node,  !- Air Distribution Unit Outlet Node Name",
        "    AirTerminal:SingleDuct:ConstantVolume:Reheat,  !- Air Terminal Object Type",
        "    Reheat Zone 2;           !- Air Terminal Name",

        "  CoilSystem:Cooling:DX,",
        "    DX Cooling Coil System 1,!- Name",
        "    CoolingCoilAvailSched,   !- Availability Schedule Name",
        "    Cooling Coil Air Inlet Node,  !- DX Cooling Coil System Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- DX Cooling Coil System Outlet Node Name",
        "    Heating Coil Air Inlet Node,  !- DX Cooling Coil System Sensor Node Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "    ACDXCoil 1;              !- Cooling Coil Name",

        "  AirTerminal:SingleDuct:ConstantVolume:Reheat,",
        "    Reheat Zone 2,           !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    Zone 2 Reheat Air Outlet Node,  !- Air Outlet Node Name",
        "    Zone 2 Reheat Air Inlet Node,  !- Air Inlet Node Name",
        "    0.52,                    !- Maximum Air Flow Rate {m3/s}",
        "    Coil:Heating:Fuel,       !- Reheat Coil Object Type",
        "    Reheat Coil Zone 2,      !- Reheat Coil Name",
        "    0.0,                     !- Maximum Hot Water or Steam Flow Rate {m3/s}",
        "    0.0,                     !- Minimum Hot Water or Steam Flow Rate {m3/s}",
        "    0.001;                   !- Convergence Tolerance",

        "  ZoneControl:Thermostat,",
        "    Zone 1 Thermostat,       !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    Dual Zone Control Type Sched,  !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    Setpoints;               !- Control 1 Name",

        "  ZoneControl:Thermostat,",
        "    Zone 2 Thermostat,       !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    Dual Zone Control Type Sched,  !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    Setpoints;               !- Control 1 Name",

        "  AirLoopHVAC:SupplyPath,",
        "    TermReheatSupplyPath,    !- Name",
        "    Zone Equipment Inlet Node,  !- Supply Air Path Inlet Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component 1 Object Type",
        "    Zone Supply Air Splitter;!- Component 1 Name",

        "  AirLoopHVAC:ReturnPath,",
        "    TermReheatReturnPath,    !- Name",
        "    Return Air Mixer Outlet, !- Return Air Path Outlet Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component 1 Object Type",
        "    Zone Return Air Mixer;   !- Component 1 Name",

        "  AirLoopHVAC:ZoneSplitter,",
        "    Zone Supply Air Splitter,!- Name",
        "    Zone Equipment Inlet Node,  !- Inlet Node Name",
        "    Zone 1 NoReheat Air Inlet Node,  !- Outlet 1 Node Name",
        "    Zone 2 Reheat Air Inlet Node;  !- Outlet 2 Node Name",

        "  AirLoopHVAC:ZoneMixer,",
        "    Zone Return Air Mixer,   !- Name",
        "    Return Air Mixer Outlet, !- Outlet Node Name",
        "    Zone 1 Outlet Node,      !- Inlet 1 Node Name",
        "    Zone 2 Outlet Node;      !- Inlet 2 Node Name",

        "  Coil:Heating:Fuel,",
        "    Main Heating Coil 1,     !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    NaturalGas,              !- Fuel Type",
        "    0.8,                     !- Burner Efficiency",
        "    45000,                   !- Nominal Capacity {W}",
        "    Heating Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Air Loop Outlet Node,    !- Air Outlet Node Name",
        "    Air Loop Outlet Node;    !- Temperature Setpoint Node Name",

        "  Coil:Heating:Fuel,",
        "    Reheat Coil Zone 2,      !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    NaturalGas,              !- Fuel Type",
        "    1.0,                     !- Burner Efficiency",
        "    3000,                    !- Nominal Capacity {W}",
        "    Zone 2 Reheat Air Inlet Node,  !- Air Inlet Node Name",
        "    Zone 2 Reheat Air Outlet Node;  !- Air Outlet Node Name",

        "  Coil:Cooling:DX:SingleSpeed,",
        "    ACDXCoil 1,              !- Name",
        "    CoolingCoilAvailSched,   !- Availability Schedule Name",
        "    20000,                   !- Gross Rated Total Cooling Capacity {W}",
        "    0.75,                    !- Gross Rated Sensible Heat Ratio",
        "    3.0,                     !- Gross Rated Cooling COP {W/W}",
        "    1.16,                    !- Rated Air Flow Rate {m3/s}",
        "    ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    Cooling Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- Air Outlet Node Name",
        "    WindACCoolCapFT,         !- Total Cooling Capacity Function of Temperature Curve Name",
        "    WindACCoolCapFFF,        !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "    WindACEIRFT,             !- Energy Input Ratio Function of Temperature Curve Name",
        "    WindACEIRFFF,            !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "    WindACPLFFPLR;           !- Part Load Fraction Correlation Curve Name",

        "  Fan:ConstantVolume,",
        "    Supply Fan 1,            !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    0.7,                     !- Fan Total Efficiency",
        "    600.0,                   !- Pressure Rise {Pa}",
        "    1.16,                     !- Maximum Flow Rate {m3/s}",
        "    0.9,                     !- Motor Efficiency",
        "    1.0,                     !- Motor In Airstream Fraction",
        "    Mixed Air Node,          !- Air Inlet Node Name",
        "    Cooling Coil Air Inlet Node;  !- Air Outlet Node Name",

        "  ZoneControl:Thermostat,",
        "    Zone 3 Thermostat,       !- Name",
        "    NORTH ZONE,               !- Zone or ZoneList Name",
        "    Dual Zone Control Type Sched,  !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    Setpoints;               !- Control 1 Name",

        "  ThermostatSetpoint:DualSetpoint,",
        "    Setpoints,               !- Name",
        "    Dual Heating Setpoints,  !- Heating Setpoint Temperature Schedule Name",
        "    Dual Cooling Setpoints;  !- Cooling Setpoint Temperature Schedule Name",

        "ScheduleTypeLimits,",
        "  HVACTemplate Any Number;                                 !- Name",

        "DesignSpecification:OutdoorAir,",
        "  SZ DSOA NORTH ZONE,                                      !- Name",
        "  flow/person,                                             !- Outdoor Air Method",
        "  0.00944,                                                 !- Outdoor Air Flow per Person {m3/s}",
        "  0.0,                                                     !- Outdoor Air Flow per Zone Floor Area {m3/s-m2}",
        "  0.0;                                                     !- Outdoor Air Flow per Zone {m3/s}",

        "Sizing:Zone,",
        "  NORTH ZONE,                                              !- Zone or ZoneList Name",
        "  SupplyAirTemperature,                                    !- Zone Cooling Design Supply Air Temperature Input Method",
        "  14,                                                      !- Zone Cooling Design Supply Air Temperature {C}",
        "  11.11,                                                   !- Zone Cooling Design Supply Air Temperature Difference {delta C}",
        "  SupplyAirTemperature,                                    !- Zone Heating Design Supply Air Temperature Input Method",
        "  50.0,                                                    !- Zone Heating Design Supply Air Temperature {C}",
        "  ,                                                        !- Zone Heating Design Supply Air Temperature Difference {delta C}",
        "  0.008,                                                   !- Zone Cooling Design Supply Air Humidity Ratio {kg-H20/kg-air}",
        "  0.008,                                                   !- Zone Heating Design Supply Air Humidity Ratio {kg-H2O/kg-air}",
        "  SZ DSOA NORTH ZONE,                                      !- Design Specification Outdoor Air Object Name",
        "  ,                                                        !- Zone Heating Sizing Factor",
        "  ,                                                        !- Zone Cooling Sizing Factor",
        "  DesignDay,                                               !- Cooling Design Air Flow Method",
        "  0,                                                       !- Cooling Design Air Flow Rate {m3/s}",
        "  ,                                                        !- Cooling Minimum Air Flow per Zone Floor Area {m3/s-m2}",
        "  ,                                                        !- Cooling Minimum Air Flow {m3/s}",
        "  0,                                                       !- Cooling Minimum Air Flow Fraction {}",
        "  DesignDay,                                               !- Heating Design Air Flow Method",
        "  0,                                                       !- Heating Design Air Flow Rate {m3/s}",
        "  ,                                                        !- Heating Maximum Air Flow per Zone Floor Area {m3/s-m2}",
        "  ,                                                        !- Heating Maximum Air Flow {m3/s}",
        "  0,                                                       !- Heating Maximum Air Flow Fraction {}",
        "  SZ DSZAD NORTH ZONE;                                     !- Design Specification Zone Air Distribution Object Name",

        "DesignSpecification:ZoneAirDistribution,",
        "  SZ DSZAD NORTH ZONE,                                     !- Name",
        "  1,                                                       !- Zone Air Distribution Effectiveness in Cooling Mode {}",
        "  1;                                                       !- Zone Air Distribution Effectiveness in Heating Mode {}",

        "ZoneHVAC:EquipmentConnections,",
        "  NORTH ZONE,                                              !- Zone Name",
        "  NORTH ZONE Equipment,                                    !- Zone Conditioning Equipment List Name",
        "  NORTH ZONE NoReheat Air Outlet Node,                     !- Zone Air Inlet Node or NodeList Name",
        "  NORTH ZONE Exhaust Node,                                                        !- Zone Air Exhaust Node or NodeList Name",
        "  NORTH ZONE Zone Air Node,                                !- Zone Air Node Name",
        "  NORTH ZONE Return Outlet;                                !- Zone Return Air Node Name",

        "ZoneHVAC:EquipmentList,",
        "  NORTH ZONE Equipment,                                    !- Name",
        "  SequentialLoad,          !- Load Distribution Scheme",
        "  ZoneHVAC:AirDistributionUnit,                     !- Zone Equipment Object Type",
        "  NORTHZONENoReheat,                                 !- Zone Equipment Name",
        "  1,                                                       !- Zone Equipment Cooling Sequence",
        "  1,                                                       !- Zone Equipment Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 1 Sequential Cooling Fraction",
        "  ,                        !- Zone Equipment 1 Sequential Heating Fraction",
        "  Fan:ZoneExhaust,         !- Zone Equipment 2 Object Type",
        "  NORTH ZONE Exhaust Fan,       !- Zone Equipment 2 Name",
        "  2,                       !- Zone Equipment 2 Cooling Sequence",
        "  2,                       !- Zone Equipment 2 Heating or No-Load Sequence",
        "  ,                        !- Zone Equipment 2 Sequential Cooling Fraction",
        "  ;                        !- Zone Equipment 2 Sequential Heating Fraction",

        "Fan:ZoneExhaust,",
        "  NORTH ZONE Exhaust Fan,       !- Name",
        "  FanAndCoilAvailSched,    !- Availability Schedule Name",
        "  0.7,                     !- Fan Total Efficiency",
        "  500,                     !- Pressure Rise {Pa}",
        "  0.15,                    !- Maximum Flow Rate {m3/s}",
        "  NORTH ZONE Exhaust Node,      !- Air Inlet Node Name",
        "  NORTH ZONE Exhaust Fan Outlet Node,  !- Air Outlet Node Name",
        "  NORTH ZONE Exhaust;            !- End-Use Subcategory",

        "  ZoneHVAC:AirDistributionUnit,",
        "    NORTHZONENoReheat,         !- Name",
        "    NORTH ZONE NoReheat Air Outlet Node,  !- Air Distribution Unit Outlet Node Name",
        "    AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "    NORTH ZONE Air Terminal;           !- Air Terminal Name",

        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "    NORTH ZONE Air Terminal,           !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    NORTH ZONE NoReheat Air Inlet Node,  !- Air Inlet Node Name",
        "    NORTH ZONE NoReheat Air Outlet Node,  !- Air Outlet Node Name",
        "    0.5;                    !- Maximum Air Flow Rate {m3/s}",

        "AirLoopHVAC,",
        "  Heat Pump 1,                                             !- Name",
        "  ,                                                        !- Controller List Name",
        "  Heat Pump 1 Availability Managers,                       !- Availability Manager List Name",
        "  0.5,                                                !- Design Supply Air Flow Rate {m3/s}",
        "  Heat Pump 1 Branches,                                    !- Branch List Name",
        "  ,                                                        !- Connector List Name",
        "  Heat Pump 1 Air Loop Inlet,                              !- Supply Side Inlet Node Name",
        "  Heat Pump 1 Return Air Outlet,                           !- Demand Side Outlet Node Name",
        "  Heat Pump 1 Supply Path Inlet,                           !- Demand Side Inlet Node Names",
        "  Heat Pump 1 Air Loop Outlet;                             !- Supply Side Outlet Node Names",

        "BranchList,",
        "  Heat Pump 1 Branches,                                    !- Name",
        "  Heat Pump 1 Main Branch;                                 !- Branch Name",

        "Branch,",
        "  Heat Pump 1 Main Branch,                                 !- Name",
        "  ,                                                        !- Pressure Drop Curve Name",
        "  AirLoopHVAC:OutdoorAirSystem,                            !- Component Object Type",
        "  Heat Pump 1 OA System,                                   !- Component Name",
        "  Heat Pump 1 Air Loop Inlet,                              !- Component Inlet Node Name",
        "  Heat Pump 1 Mixed Air Outlet,                            !- Component Outlet Node Name",
        "  AirLoopHVAC:UnitaryHeatPump:AirToAir,                    !- Component Object Type",
        "  Heat Pump 1 Heat Pump,                                   !- Component Name",
        "  Heat Pump 1 Mixed Air Outlet,                            !- Component Inlet Node Name",
        "  Heat Pump 1 Air Loop Outlet;                             !- Component Outlet Node Name",

        "AirLoopHVAC:SupplyPath,",
        "  Heat Pump 1 Supply Path,                                 !- Name",
        "  Heat Pump 1 Supply Path Inlet,                           !- Supply Air Path Inlet Node Name",
        "  AirLoopHVAC:ZoneSplitter,                                !- Component Object Type",
        "  Heat Pump 1 Zone Splitter;                               !- Component Name",

        "AirLoopHVAC:ZoneSplitter,",
        "  Heat Pump 1 Zone Splitter,                               !- Name",
        "  Heat Pump 1 Supply Path Inlet,                           !- Inlet Node Name",
        "  NORTH ZONE NoReheat Air Inlet Node;                      !- Outlet Node Name",

        "AirLoopHVAC:ReturnPath,",
        "  Heat Pump 1 Return Path,                                 !- Name",
        "  Heat Pump 1 Return Air Outlet,                           !- Return Air Path Outlet Node Name",
        "  AirLoopHVAC:ZoneMixer,                                   !- Component Object Type",
        "  Heat Pump 1 Zone Mixer;                                  !- Component Name",

        "AirLoopHVAC:ZoneMixer,",
        "  Heat Pump 1 Zone Mixer,                                  !- Name",
        "  Heat Pump 1 Return Air Outlet,                           !- Outlet Node Name",
        "  NORTH ZONE Return Outlet;                                !- Inlet Node Name",

        "AvailabilityManagerAssignmentList,",
        "  Heat Pump 1 Availability Managers,                       !- Name",
        "  AvailabilityManager:NightCycle,                          !- Availability Manager Object Type",
        "  Heat Pump 1 Availability;                                !- Availability Manager Name",

        "AvailabilityManager:NightCycle,",
        "  Heat Pump 1 Availability,                                !- Name",
        "  HVACTemplate-Always 1,                                   !- Applicability Schedule Name",
        "  FanAndCoilAvailSched,                                           !- Fan Schedule Name",
        "  CycleOnAny,                                              !- Control Type",
        "  0.2,                                                     !- Thermostat Tolerance {deltaC}",
        "  FixedRunTime,                                            !- Cycling Run Time Control Type",
        "  3600,                                                    !- Cycling run time {s}",
        "  ;                                                        !- Control Zone Name",

        "Schedule:Compact,",
        "  HVACTemplate-Always 1,                                   !- Name",
        "  HVACTemplate Any Number,                                 !- Schedule Type Limits Name",
        "  Through: 12/31,                                          !- Field 1",
        "  For: AllDays,                                            !- Field 2",
        "  Until: 24:00,                                            !- Field 3",
        "  1;                                                       !- Field 4",

        "AirLoopHVAC:UnitaryHeatPump:AirToAir,",
        "  Heat Pump 1 Heat Pump,                                   !- Name",
        "  ,                                                        !- Availability Schedule Name",
        "  Heat Pump 1 Mixed Air Outlet,                            !- Air Inlet Node Name",
        "  Heat Pump 1 Air Loop Outlet,                             !- Air Outlet Node Name",
        "  0.5,                                                !- Cooling Supply Air Flow Rate",
        "  0.5,                                                !- Heating Supply Air Flow Rate",
        "  0.0,                                                !- No Load Supply Air Flow Rate",
        "  NORTH ZONE,                                              !- Controlling Zone or Thermostat Location",
        "  Fan:OnOff,                                               !- Supply Air Fan Object Type",
        "  Heat Pump 1 Supply Fan,                                  !- Supply Air Fan Name",
        "  Coil:Heating:DX:SingleSpeed,                             !- Heating Coil Object Type",
        "  Heat Pump 1 HP Heating Coil,                             !- Heating Coil Name",
        "  Coil:Cooling:DX:SingleSpeed,                             !- Cooling Coil Object Type",
        "  Heat Pump 1 Cooling Coil,                                !- Cooling Coil Name",
        "  Coil:Heating:Electric,                                   !- Supplemental Heating Coil Object Type",
        "  Heat Pump 1 Sup Heat Coil,                               !- Supplemental Heating Coil Name",
        "  80.0,                                                !- Maximum Supply Air Temperature from Supplemental Heater",
        "  21,                                                      !- Maximum Outdoor Dry-Bulb Temperature for Supplemental Heater Operation",
        "  BlowThrough,                                             !- Fan Placement",
        "  HVACTemplate-Always 0;                                   !- Supply Air Fan Operating Mode Schedule Name",

        "Schedule:Compact,",
        "  HVACTemplate-Always 0,                                   !- Name",
        "  HVACTemplate Any Number,                                 !- Schedule Type Limits Name",
        "  Through: 12/31,                                          !- Field 1",
        "  For: AllDays,                                            !- Field 2",
        "  Until: 24:00,                                            !- Field 3",
        "  0;                                                       !- Field 4",

        "Coil:Heating:DX:SingleSpeed,",
        "  Heat Pump 1 HP Heating Coil,                             !- Name",
        "  ,                                                        !- Availability Schedule Name",
        "  9000.0,                                                  !- Rated Total Heating Capacity {W}",
        "  2.75,                                                    !- Rated COP",
        "  0.5,                                                     !- Rated Air Flow Rate {m3/s}",
        "  ,                                                        !- 2017 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  ,                                                        !- 2023 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  Heat Pump 1 Cooling Coil Outlet,                         !- Air Inlet Node Name",
        "  Heat Pump 1 Heating Coil Outlet,                         !- Air Outlet Node Name",
        "  Heat Pump 1 HP Heating Coil Cap-FT,                      !- Total Heating Capacity Function of Temperature Curve Name",
        "  Heat Pump 1 HP Heating Coil Cap-FF,                      !- Total Heating Capacity Function of Flow Fraction Curve Name",
        "  Heat Pump 1 HP Heating Coil EIR-FT,                      !- Energy Input Ratio Function of Temperature Curve Name",
        "  Heat Pump 1 HP Heating Coil EIR-FF,                      !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  Heat Pump 1 HP Heating Coil PLF,                         !- Part Load Fraction Correlation Curve Name",
        "  Heat Pump 1 HP Heating Coil DefrEIR-FT,                  !- Defrost Energy Input Ratio Function of Temperature Curve Name",
        "  -8,                                                      !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  ,                                                        !- Outdoor Dry-Bulb Temperature to Turn On Compressor",
        "  5,                                                       !- Maximum Outdoor Dry-Bulb Temperature for Defrost Operation {C}",
        "  0,                                                       !- Crankcase Heater Capacity {W}",
        "  0,                                                       !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "  ReverseCycle,                                            !- Defrost Strategy",
        "  Timed,                                                   !- Defrost Control",
        "  0.058333,                                                !- Defrost Time Period Fraction",
        "  10.0;                                                !- Resistive Defrost Heater Capacity {W}",

        "Curve:Cubic,",
        "  Heat Pump 1 HP Heating Coil Cap-FT,                      !- Name",
        "  0.758746,                                                !- Coefficient1 Constant",
        "  0.027626,                                                !- Coefficient2 x",
        "  0.000148716,                                             !- Coefficient3 x**2",
        "  0.0000034992,                                            !- Coefficient4 x**3",
        "  -20.0,                                                   !- Minimum Value of x",
        "  20.0;                                                    !- Maximum Value of x",

        "Curve:Cubic,",
        "  Heat Pump 1 HP Heating Coil Cap-FF,                      !- Name",
        "  0.84,                                                    !- Coefficient1 Constant",
        "  0.16,                                                    !- Coefficient2 x",
        "  0.0,                                                     !- Coefficient3 x**2",
        "  0.0,                                                     !- Coefficient4 x**3",
        "  0.5,                                                     !- Minimum Value of x",
        "  1.5;                                                     !- Maximum Value of x",

        "Curve:Cubic,",
        "  Heat Pump 1 HP Heating Coil EIR-FT,                      !- Name",
        "  1.19248,                                                 !- Coefficient1 Constant",
        "  -0.0300438,                                              !- Coefficient2 x",
        "  0.00103745,                                              !- Coefficient3 x**2",
        "  -0.000023328,                                            !- Coefficient4 x**3",
        "  -20.0,                                                   !- Minimum Value of x",
        "  20.0;                                                    !- Maximum Value of x",

        "Curve:Quadratic,",
        "  Heat Pump 1 HP Heating Coil EIR-FF,                      !- Name",
        "  1.3824,                                                  !- Coefficient1 Constant",
        "  -0.4336,                                                 !- Coefficient2 x",
        "  0.0512,                                                  !- Coefficient3 x**2",
        "  0.0,                                                     !- Minimum Value of x",
        "  1.0;                                                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  Heat Pump 1 HP Heating Coil PLF,                         !- Name",
        "  0.75,                                                    !- Coefficient1 Constant",
        "  0.25,                                                    !- Coefficient2 x",
        "  0.0,                                                     !- Coefficient3 x**2",
        "  0.0,                                                     !- Minimum Value of x",
        "  1.0;                                                     !- Maximum Value of x",

        "Curve:Biquadratic,",
        "  Heat Pump 1 HP Heating Coil DefrEIR-FT,                  !- Name",
        "  1,                                                       !- Coefficient1 Constant",
        "  0,                                                       !- Coefficient2 x",
        "  0,                                                       !- Coefficient3 x**2",
        "  0,                                                       !- Coefficient4 y",
        "  0,                                                       !- Coefficient5 y**2",
        "  0,                                                       !- Coefficient6 x*y",
        "  0,                                                       !- Minimum Value of x",
        "  50,                                                      !- Maximum Value of x",
        "  0,                                                       !- Minimum Value of y",
        "  50;                                                      !- Maximum Value of y",

        "Coil:Heating:Electric,",
        "  Heat Pump 1 Sup Heat Coil,                               !- Name",
        "  ,                                                        !- Availability Schedule Name",
        "  1,                                                       !- Efficiency",
        "  12000.0,                                                !- Nominal Capacity of the Coil {W}",
        "  Heat Pump 1 Heating Coil Outlet,                         !- Air Inlet Node Name",
        "  Heat Pump 1 Air Loop Outlet,                             !- Air Outlet Node Name",
        "  ;                                                        !- Coil Temp Setpoint Node",

        "Coil:Cooling:DX:SingleSpeed,",
        "  Heat Pump 1 Cooling Coil,                                !- Name",
        "  ,                                                        !- Availability Schedule Name",
        "  9000.0,                                                  !- Gross Rated Total Cooling Capacity {W}",
        "  0.8,                                                     !- Gross Rated Sensible Heat Ratio",
        "  3,                                                       !- Rated COP",
        "  0.5,                                                     !- Rated Air Flow Rate {m3/s}",
        "  ,                                                        !- 2017 Rated Evaporator Fan Power per Volume Flow Rate {W/(m3/s)}",
        "  ,                                                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "  Heat Pump 1 Supply Fan Outlet,                           !- Air Inlet Node Name",
        "  Heat Pump 1 Cooling Coil Outlet,                         !- Air Outlet Node Name",
        "  Heat Pump 1 Cool Coil Cap-FT,                            !- Total Cooling Capacity Function of Temperature Curve Name",
        "  Heat Pump 1 Cool Coil Cap-FF,                            !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "  Heat Pump 1 Cool Coil EIR-FT,                            !- Energy Input Ratio Function of Temperature Curve Name",
        "  Heat Pump 1 Cool Coil EIR-FF,                            !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "  Heat Pump 1 Cool Coil PLF,                               !- Part Load Fraction Correlation Curve Name",
        "  ,                                                        !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "  0,                                                       !- Nominal Time for Condensate Removal to Begin",
        "  0,                                                       !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity",
        "  0,                                                       !- Maximum Cycling Rate",
        "  0,                                                       !- Latent Capacity Time Constant",
        "  Heat Pump 1 Cooling Coil Condenser Inlet,                !- Condenser Air Inlet Node Name",
        "  AirCooled,                                               !- Condenser Type",
        "  0,                                                       !- Evaporative Condenser Effectiveness",
        "  ,                                                        !- Evaporative Condenser Air Flow Rate",
        "  0,                                                       !- Evaporative Condenser Pump Rated Power Consumption",
        "  0,                                                       !- Crankcase Heater Capacity",
        "  10;                                                      !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation",

        "Curve:Biquadratic,",
        "  Heat Pump 1 Cool Coil Cap-FT,                            !- Name",
        "  0.942587793,                                             !- Coefficient1 Constant",
        "  0.009543347,                                             !- Coefficient2 x",
        "  0.00068377,                                              !- Coefficient3 x**2",
        "  -0.011042676,                                            !- Coefficient4 y",
        "  0.000005249,                                             !- Coefficient5 y**2",
        "  -0.00000972,                                             !- Coefficient6 x*y",
        "  12.77778,                                                !- Minimum Value of x",
        "  23.88889,                                                !- Maximum Value of x",
        "  18.0,                                                    !- Minimum Value of y",
        "  46.11111;                                                !- Maximum Value of y",

        "Curve:Quadratic,",
        "  Heat Pump 1 Cool Coil Cap-FF,                            !- Name",
        "  0.8,                                                     !- Coefficient1 Constant",
        "  0.2,                                                     !- Coefficient2 x",
        "  0,                                                       !- Coefficient3 x**2",
        "  0.5,                                                     !- Minimum Value of x",
        "  1.5;                                                     !- Maximum Value of x",

        "Curve:Biquadratic,",
        "  Heat Pump 1 Cool Coil EIR-FT,                            !- Name",
        "  0.342414409,                                             !- Coefficient1 Constant",
        "  0.034885008,                                             !- Coefficient2 x",
        "  -0.0006237,                                              !- Coefficient3 x**2",
        "  0.004977216,                                             !- Coefficient4 y",
        "  0.000437951,                                             !- Coefficient5 y**2",
        "  -0.000728028,                                            !- Coefficient6 x*y",
        "  12.77778,                                                !- Minimum Value of x",
        "  23.88889,                                                !- Maximum Value of x",
        "  18.0,                                                    !- Minimum Value of y",
        "  46.11111;                                                !- Maximum Value of y",

        "Curve:Quadratic,",
        "  Heat Pump 1 Cool Coil EIR-FF,                            !- Name",
        "  1.1552,                                                  !- Coefficient1 Constant",
        "  -0.1808,                                                 !- Coefficient2 x",
        "  0.0256,                                                  !- Coefficient3 x**2",
        "  0.5,                                                     !- Minimum Value of x",
        "  1.5;                                                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "  Heat Pump 1 Cool Coil PLF,                               !- Name",
        "  0.85,                                                    !- Coefficient1 Constant",
        "  0.15,                                                    !- Coefficient2 x",
        "  0,                                                       !- Coefficient3 x**2",
        "  0,                                                       !- Minimum Value of x",
        "  1;                                                       !- Maximum Value of x",

        "OutdoorAir:Node,",
        "  Heat Pump 1 Cooling Coil Condenser Inlet,                !- Name",
        "  -1;                                                      !- Height Above Ground",

        "Fan:OnOff,",
        "  Heat Pump 1 Supply Fan,                                  !- Name",
        "  FanAndCoilAvailSched,                                           !- Availability Schedule Name",
        "  0.7,                                                     !- Fan Efficiency",
        "  600,                                                     !- Pressure Rise {Pa}",
        "  0.5,                                                !- Maximum Flow Rate {m3/s}",
        "  0.9,                                                     !- Motor Efficiency",
        "  1,                                                       !- Motor in Airstream Fraction",
        "  Heat Pump 1 Mixed Air Outlet,                            !- Air Inlet Node Name",
        "  Heat Pump 1 Supply Fan Outlet;                           !- Air Outlet Node Name",

        "OutdoorAir:NodeList,",
        "  Heat Pump 1 Outside Air Inlet;                           !- Node or NodeList Name 1",

        "AirLoopHVAC:OutdoorAirSystem,",
        "  Heat Pump 1 OA System,                                   !- Name",
        "  Heat Pump 1 OA System Controllers,                       !- Controller List Name",
        "  Heat Pump 1 OA System Equipment;                         !- Outdoor Air Equipment List Name",

        "AirLoopHVAC:ControllerList,",
        "  Heat Pump 1 OA System Controllers,                       !- Name",
        "  Controller:OutdoorAir,                                   !- Controller Object Type",
        "  Heat Pump 1 OA Controller;                               !- Controller Name",

        "AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "  Heat Pump 1 OA System Equipment,                         !- Name",
        "  OutdoorAir:Mixer,                                        !- Component Object Type",
        "  Heat Pump 1 OA Mixing Box;                               !- Component Name",

        "OutdoorAir:Mixer,",
        "  Heat Pump 1 OA Mixing Box,                               !- Name",
        "  Heat Pump 1 Mixed Air Outlet,                            !- Mixed Air Node Name",
        "  Heat Pump 1 Outside Air Inlet,                           !- Outside Air Stream Node Name",
        "  Heat Pump 1 Relief Air Outlet,                           !- Relief Air Stream Node Name",
        "  Heat Pump 1 Air Loop Inlet;                              !- Return Air Stream Node Name",

        "SetpointManager:SingleZone:Cooling,",
        "  Heat Pump 1 Economizer Supply Air Temp Manager,          !- Name",
        "  Temperature,                                             !- Control Variable",
        "  13,                                                      !- minimum supply air temperature {C}",
        "  45,                                                      !- maximum supply air temperature {C}",
        "  NORTH ZONE,                                              !- Control Zone Name",
        "  NORTH ZONE Zone Air Node,                                !- Zone Node Name",
        "  NORTH ZONE NoReheat Air Outlet Node,                     !- Zone Inlet Node Name",
        "  Heat Pump 1 Air Loop Outlet;                             !- Setpoint Node or NodeList Name",

        "SetpointManager:MixedAir,",
        "  Heat Pump 1 Cooling Coil Air Temp Manager,               !- Name",
        "  Temperature,                                             !- Control Variable",
        "  Heat Pump 1 Air Loop Outlet,                             !- Reference Setpoint Node Name",
        "  Heat Pump 1 Mixed Air Outlet,                            !- Fan Inlet Node Name",
        "  Heat Pump 1 Supply Fan Outlet,                           !- Fan Outlet Node Name",
        "  Heat Pump 1 Mixed Air Outlet;                            !- Setpoint Node or NodeList Name",

        "Controller:OutdoorAir,",
        "  Heat Pump 1 OA Controller,                               !- Name",
        "  Heat Pump 1 Relief Air Outlet,                           !- Relief Air Outlet Node Name",
        "  Heat Pump 1 Air Loop Inlet,                              !- Return Air Node Name",
        "  Heat Pump 1 Mixed Air Outlet,                            !- Mixed Air Node Name",
        "  Heat Pump 1 Outside Air Inlet,                           !- Actuator Node Name",
        "  0.1,                                                !- Minimum Outdoor Air Flow Rate {m3/s}",
        "  0.3,                                                !- Maximum Outdoor Air Flow Rate {m3/s}",
        "  DifferentialDryBulb,                                     !- Economizer Control Type",
        "  ModulateFlow,                                            !- Economizer Control Action Type",
        "  19,                                                      !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "  ,                                                        !- Economizer Maximum Limit Enthalpy {J/kg}",
        "  ,                                                        !- Economizer Maximum Limit Dewpoint Temperature (C)",
        "  ,                                                        !- Electronic Enthalpy Limit Curve Name",
        "  ,                                                        !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "  NoLockout,                                               !- Lockout Type",
        "  FixedMinimum,                                            !- Minimum Limit Type",
        "  Minimum OA Sch,                                            !- Minimum Outdoor Air Schedule Name",
        "  ,                                                        !- Minimum Fraction of Outdoor Air Schedule Name",
        "  ,                                                        !- Maximum Fraction of Outdoor Air Schedule Name",
        "  ;                                                        !- Mechanical Ventilation Controller Name",

        "Site:GroundTemperature:BuildingSurface,",
        "  20.03,                                                   !- January Ground Temperature",
        "  20.03,                                                   !- February Ground Temperature",
        "  20.13,                                                   !- March Ground Temperature",
        "  20.30,                                                   !- April Ground Temperature",
        "  20.43,                                                   !- May Ground Temperature",
        "  20.52,                                                   !- June Ground Temperature",
        "  20.62,                                                   !- July Ground Temperature",
        "  20.77,                                                   !- August Ground Temperature",
        "  20.78,                                                   !- September Ground Temperature",
        "  20.55,                                                   !- October Ground Temperature",
        "  20.44,                                                   !- November Ground Temperature",
        "  20.20;                                                   !- December Ground Temperature",

        "Output:Diagnostics,DisplayExtraWarnings,DisplayUnusedSchedules;"});

    ASSERT_TRUE(process_idf(idf_objects));

    bool ErrorsFound = false;
    // Read objects
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetWindowGlassSpectralData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    Material::GetMaterialData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    SurfaceGeometry::GetGeometryParameters(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosBldgRotAppGonly = 1.0;
    state->dataSurfaceGeometry->SinBldgRotAppGonly = 0.0;
    SurfaceGeometry::GetSurfaceData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    // Read AirflowNetwork inputs
    state->afn->get_input();

    Real64 PresssureSet = 0.5;
    // Assign values
    state->dataScheduleMgr->Schedule(1).CurrentValue = 25.55;         // WindowVentSched
    state->dataScheduleMgr->Schedule(9).CurrentValue = 1.0;           // FanAndCoilAvailSched
    state->dataScheduleMgr->Schedule(14).CurrentValue = 1.0;          // VentingSched
    state->dataScheduleMgr->Schedule(16).CurrentValue = PresssureSet; // Pressure setpoint
    state->dataScheduleMgr->Schedule(17).CurrentValue = 1.0;          // HVACTemplate-Always 1
    state->dataScheduleMgr->Schedule(18).CurrentValue = 0.0;          // HVACTemplate-Always 0

    state->afn->AirflowNetworkFanActivated = true;
    state->dataEnvrn->OutDryBulbTemp = -17.29025;
    state->dataEnvrn->OutHumRat = 0.0008389;
    state->dataEnvrn->OutBaroPress = 99063.0;
    state->dataEnvrn->WindSpeed = 4.9;
    state->dataEnvrn->WindDir = 270.0;
    state->dataEnvrn->StdRhoAir = 1.2;
    state->dataHVACGlobal->TimeStepSys = 0.1;

    for (int i = 1; i <= 50; ++i) {
        state->afn->AirflowNetworkNodeSimu(i).TZ = 23.0;
        state->afn->AirflowNetworkNodeSimu(i).WZ = 0.0008400;
        if ((i > 4 && i < 10) || i == 32) {
            state->afn->AirflowNetworkNodeSimu(i).TZ = DataEnvironment::OutDryBulbTempAt(*state, state->afn->AirflowNetworkNodeData(i).NodeHeight);
            state->afn->AirflowNetworkNodeSimu(i).WZ = state->dataEnvrn->OutHumRat;
        }
    }

    // Set up node values
    state->dataLoopNodes->Node.allocate(17);
    state->dataLoopNodes->Node(state->afn->MultizoneCompExhaustFanData(1).InletNode).MassFlowRate = 0.1005046;

    state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(1).InletNode).MassFlowRate = 1.40;
    state->afn->DisSysCompCVFData(1).FlowRate = state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(1).InletNode).MassFlowRate;
    state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(2).InletNode).MassFlowRate = 0.52;
    state->afn->DisSysCompCVFData(2).FlowRate = state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(2).InletNode).MassFlowRate;

    state->afn->DisSysCompOutdoorAirData(2).InletNode = 1;
    state->dataLoopNodes->Node(state->afn->DisSysCompOutdoorAirData(2).InletNode).MassFlowRate = 0.2795108;
    state->afn->DisSysCompOutdoorAirData(1).InletNode = 6;
    state->dataLoopNodes->Node(state->afn->DisSysCompOutdoorAirData(1).InletNode).MassFlowRate = 0.1095108;

    if (state->afn->DisSysCompReliefAirData(1).InletNode == 0) {
        state->afn->DisSysCompReliefAirData(1).OutletNode = 1;
    }

    state->afn->AirflowNetworkNodeData(3).AirLoopNum = 1;
    state->afn->AirflowNetworkLinkageData(51).AirLoopNum = 1;
    state->afn->AirflowNetworkLinkageData(52).AirLoopNum = 1;
    state->afn->AirflowNetworkLinkageData(66).AirLoopNum = 2;
    state->afn->AirflowNetworkLinkageData(42).AirLoopNum = 1;
    state->afn->AirflowNetworkLinkageData(67).AirLoopNum = 2;

    state->dataAirLoop->AirLoopAFNInfo.allocate(2);
    state->dataAirLoop->AirLoopAFNInfo(1).LoopFanOperationMode = 0.0;
    state->dataAirLoop->AirLoopAFNInfo(2).LoopFanOperationMode = 1.0;
    state->dataAirLoop->AirLoopAFNInfo(1).LoopOnOffFanPartLoadRatio = 0.0;
    state->dataAirLoop->AirLoopAFNInfo(2).LoopOnOffFanPartLoadRatio = 1.0;
    state->dataAirLoop->AirLoopAFNInfo(2).LoopSystemOnMassFlowrate = 0.52;

    state->afn->ANZT = 0.0;
    state->afn->ANZW = 0.0;
    state->afn->calculate_balance();

    // Check mass flow rate
    EXPECT_NEAR(1.40, state->afn->AirflowNetworkLinkSimu(42).FLOW, 0.0001);
    EXPECT_NEAR(0.52, state->afn->AirflowNetworkLinkSimu(67).FLOW, 0.0001);
    EXPECT_NEAR(0.2795108, state->afn->AirflowNetworkLinkSimu(51).FLOW, 0.0001);
    EXPECT_NEAR(0.1095108, state->afn->AirflowNetworkLinkSimu(66).FLOW, 0.0001);
    EXPECT_NEAR(0.1005046, state->afn->AirflowNetworkLinkSimu(15).FLOW, 0.0001);

    state->afn->AirflowNetworkFanActivated = false;
    // #7977
    state->afn->calculate_balance();
    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(5);
    state->dataZoneEquip->ZoneEquipConfig.allocate(5);
    for (auto &thisZoneHB : state->dataZoneTempPredictorCorrector->zoneHeatBalance) {
        thisZoneHB.MAT = 23.0;
        thisZoneHB.ZoneAirHumRat = 0.001;
        thisZoneHB.ZoneAirHumRatAvg = 0.001;
    }
    state->dataHeatBal->Zone(1).OutDryBulbTemp = state->dataEnvrn->OutDryBulbTemp;
    state->dataHeatBal->Zone(2).OutDryBulbTemp = state->dataEnvrn->OutDryBulbTemp;
    state->dataHeatBal->Zone(3).OutDryBulbTemp = state->dataEnvrn->OutDryBulbTemp;
    state->dataHeatBal->Zone(4).OutDryBulbTemp = state->dataEnvrn->OutDryBulbTemp;
    state->dataHeatBal->Zone(5).OutDryBulbTemp = state->dataEnvrn->OutDryBulbTemp;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = false;
    state->dataZoneEquip->ZoneEquipConfig(2).IsControlled = false;
    state->dataZoneEquip->ZoneEquipConfig(3).IsControlled = false;
    state->dataZoneEquip->ZoneEquipConfig(4).IsControlled = false;
    state->dataZoneEquip->ZoneEquipConfig(5).IsControlled = false;
    state->afn->exchangeData.allocate(5);
    state->afn->AirflowNetworkLinkSimu(3).FLOW2 = 0.002364988;
    state->afn->report();

    EXPECT_NEAR(state->afn->AirflowNetworkReportData(1).MultiZoneInfiSenLossW, 95.89575, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkReportData(1).MultiZoneInfiLatLossW, 0.969147, 0.001);

    state->afn->AirflowNetworkCompData(state->afn->AirflowNetworkLinkageData(2).CompNum).CompTypeNum = AirflowNetwork::iComponentTypeNum::DOP;
    state->afn->report();

    EXPECT_NEAR(state->afn->AirflowNetworkReportData(1).MultiZoneVentSenLossW, 95.89575, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkReportData(1).MultiZoneVentLatLossW, 0.969147, 0.001);
    // #8475
    state->dataHVACGlobal->TimeStepSys = 0.1;
    state->dataHVACGlobal->TimeStepSysSec = state->dataHVACGlobal->TimeStepSys * Constant::SecInHour;
    state->dataHeatBal->Zone(1).Volume = 30.0;
    // Ventilation
    state->afn->update();
    state->afn->report();
    EXPECT_NEAR(state->afn->exchangeData(1).SumMVCp, 2.38012, 0.001);
    EXPECT_NEAR(state->afn->exchangeData(1).SumMVCpT, -41.1529, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkZnRpt(1).VentilVolume, 0.7314456, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkZnRpt(1).VentilAirChangeRate, 0.2438, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkZnRpt(1).VentilMass, 0.85114, 0.001);
    // Infiltration
    state->afn->AirflowNetworkCompData(state->afn->AirflowNetworkLinkageData(2).CompNum).CompTypeNum = AirflowNetwork::iComponentTypeNum::SCR;
    state->afn->update();
    state->afn->report();
    EXPECT_NEAR(state->afn->exchangeData(1).SumMCp, 2.38012, 0.001);
    EXPECT_NEAR(state->afn->exchangeData(1).SumMCpT, -41.1529, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkZnRpt(1).InfilVolume, 0.7314456, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkZnRpt(1).InfilAirChangeRate, 0.2438, 0.001);
    EXPECT_NEAR(state->afn->AirflowNetworkZnRpt(1).InfilMass, 0.85114, 0.001);
}

TEST_F(EnergyPlusFixture, AirflowNetwork_CheckNumOfFansInAirLoopTest)
{
    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 3;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(3);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(2).TypeOf = "Fan:VariableVolume";
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).Name = "CVF";
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(2).Name = "VAV";

    ASSERT_THROW(state->afn->validate_distribution(), std::runtime_error);

    std::string const error_string = delimited_string({
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: An AirLoop branch, , has two or more fans: CVF,VAV",
        "   **   ~~~   ** The AirflowNetwork model allows a single supply fan in an AirLoop only. Please make "
        "changes in the input file accordingly.",
        "   **  Fatal  ** AirflowNetwork::Solver::validate_distribution: Program terminates for preceding reason(s).",
        "   ...Summary of Errors that led to program termination:",
        "   ..... Reference severe error count=1",
        "   ..... Last severe error=AirflowNetwork::Solver::validate_distribution: An AirLoop branch, , has two or more fans: CVF,VAV",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

// Missing an AirflowNetwork:Distribution:Node for the Zone Air Node
TEST_F(EnergyPlusFixture, AirflowNetwork_CheckMultiZoneNodes_NoZoneNode)
{
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ATTIC ZONE";

    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ATTIC ZONE";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ATTIC ZONE";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    state->dataLoopNodes->NumOfNodes = 1;
    state->dataLoopNodes->Node.allocate(1);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(1);
    state->dataLoopNodes->NodeID(1) = "ATTIC ZONE AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ATTIC ZONE AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ATTIC ZONE";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;

    ASSERT_THROW(state->afn->validate_distribution(), std::runtime_error);

    std::string const error_string = delimited_string({
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'ATTIC ZONE AIR NODE' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   **   ~~~   ** This Node is the zone air node for Zone 'ATTIC ZONE'.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'ATTIC ZONE AIR NODE' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   **  Fatal  ** AirflowNetwork::Solver::validate_distribution: Program terminates for preceding reason(s).",
        "   ...Summary of Errors that led to program termination:",
        "   ..... Reference severe error count=2",
        "   ..... Last severe error=AirflowNetwork::Solver::validate_distribution: 'ATTIC ZONE AIR NODE' is not defined as an "
        "AirflowNetwork:Distribution:Node object.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

// Can't find an inlet node for a Zone referenced in AirflowNetwork:MultiZone:Zone object
TEST_F(EnergyPlusFixture, AirflowNetwork_CheckMultiZoneNodes_NoInletNode)
{
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ATTIC ZONE";

    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ATTIC ZONE";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ATTIC ZONE";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    state->dataLoopNodes->NumOfNodes = 1;
    state->dataLoopNodes->Node.allocate(2);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(1);
    state->dataLoopNodes->NodeID(1) = "ATTIC ZONE AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ATTIC ZONE AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ATTIC ZONE";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;

    // One AirflowNetwork:MultiZone:Zone object
    state->afn->AirflowNetworkNumOfZones = 1;
    state->afn->MultizoneZoneData.allocate(1);
    state->afn->MultizoneZoneData(1).ZoneNum = 1;
    state->afn->MultizoneZoneData(1).ZoneName = "ATTIC ZONE";

    // Assume only one AirflowNetwork:Distribution:Node object is set for the Zone Air Node
    state->afn->AirflowNetworkNumOfNodes = 1;
    state->afn->AirflowNetworkNodeData.allocate(1);
    state->afn->AirflowNetworkNodeData(1).Name = "ATTIC ZONE";
    state->afn->AirflowNetworkNodeData(1).EPlusZoneNum = 1;

    state->afn->SplitterNodeNumbers.allocate(2);
    state->afn->SplitterNodeNumbers(1) = 0;
    state->afn->SplitterNodeNumbers(2) = 0;

    // MixedAir::NumOAMixers.allocate(1);
    state->afn->validate_distribution();

    EXPECT_TRUE(compare_err_stream("", true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_DuplicatedNodeNameTest)
{

    std::string const idf_objects = delimited_string({
        "  SimulationControl,",
        "    No,                      !- Do Zone Sizing Calculation",
        "    No,                      !- Do System Sizing Calculation",
        "    No,                      !- Do Plant Sizing Calculation",
        "    No,                      !- Run Simulation for Sizing Periods",
        "    Yes;                     !- Run Simulation for Weather File Run Periods",

        "  Building,",
        "    Exercise 1A,             !- Name",
        "    0.0,                     !- North Axis {deg}",
        "    Country,                 !- Terrain",
        "    0.04,                    !- Loads Convergence Tolerance Value",
        "    0.4,                     !- Temperature Convergence Tolerance Value {deltaC}",
        "    FullInteriorAndExterior, !- Solar Distribution",
        "    ,                        !- Maximum Number of Warmup Days",
        "    6;                       !- Minimum Number of Warmup Days",

        "  SurfaceConvectionAlgorithm:Inside,",
        "    TARP;                    !- Algorithm",

        "  SurfaceConvectionAlgorithm:Outside,",
        "    TARP;                    !- Algorithm",

        "  HeatBalanceAlgorithm,",
        "    ConductionTransferFunction;  !- Algorithm",

        "  Timestep,",
        "    4;                       !- Number of Timesteps per Hour",

        "  Site:Location,",
        "    Pheonix,                 !- Name",
        "    33.43,                   !- Latitude {deg}",
        "    -112.02,                 !- Longitude {deg}",
        "    -7.0,                    !- Time Zone {hr}",
        "    339.0;                   !- Elevation {m}",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Cooling .4% Conditions DB=>MWB,  !- Name",
        "    7,                       !- Month",
        "    21,                      !- Day of Month",
        "    SummerDesignDay,         !- Day Type",
        "    32.80000,                !- Maximum Dry-Bulb Temperature {C}",
        "    10.90000,                !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    23.60000,                !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.21,                !- Barometric Pressure {Pa}",
        "    0.0,                     !- Wind Speed {m/s}",
        "    0.0,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    1.000000;                !- Sky Clearness",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Heating 99.6% Conditions,  !- Name",
        "    1,                       !- Month",
        "    21,                      !- Day of Month",
        "    WinterDesignDay,         !- Day Type",
        "    -21.20000,               !- Maximum Dry-Bulb Temperature {C}",
        "    0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    -21.20000,               !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.21,                !- Barometric Pressure {Pa}",
        "    4.600000,                !- Wind Speed {m/s}",
        "    270.0000,                !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    0.0;                     !- Sky Clearness",

        "  RunPeriod,",
        "    RP1,                     !- Name",
        "    1,                       !- Begin Month",
        "    1,                       !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    12,                      !- End Month",
        "    31,                      !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  Site:GroundTemperature:BuildingSurface,",
        "    23.0,                    !- January Ground Temperature {C}",
        "    23.0,                    !- February Ground Temperature {C}",
        "    23.0,                    !- March Ground Temperature {C}",
        "    23.0,                    !- April Ground Temperature {C}",
        "    23.0,                    !- May Ground Temperature {C}",
        "    23.0,                    !- June Ground Temperature {C}",
        "    23.0,                    !- July Ground Temperature {C}",
        "    23.0,                    !- August Ground Temperature {C}",
        "    23.0,                    !- September Ground Temperature {C}",
        "    23.0,                    !- October Ground Temperature {C}",
        "    23.0,                    !- November Ground Temperature {C}",
        "    23.0;                    !- December Ground Temperature {C}",

        "  ScheduleTypeLimits,",
        "    Temperature,             !- Name",
        "    -60,                     !- Lower Limit Value",
        "    200,                     !- Upper Limit Value",
        "    CONTINUOUS,              !- Numeric Type",
        "    Temperature;             !- Unit Type",

        "  ScheduleTypeLimits,",
        "    Control Type,            !- Name",
        "    0,                       !- Lower Limit Value",
        "    4,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "  ScheduleTypeLimits,",
        "    Fraction,                !- Name",
        "    0.0,                     !- Lower Limit Value",
        "    1.0,                     !- Upper Limit Value",
        "    CONTINUOUS;              !- Numeric Type",

        "  Schedule:Compact,",
        "    HVACAvailSched,          !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    1.0;                     !- Field 4",

        "  Schedule:Compact,",
        "    Dual Heating Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    23.0;                    !- Field 4",

        "  Schedule:Compact,",
        "    Dual Cooling Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    23.0;                    !- Field 4",

        "  Schedule:Compact,",
        "    Dual Zone Control Type Sched,  !- Name",
        "    Control Type,            !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    4;                       !- Field 4",

        "  Material,",
        "    Gypsum Board,            !- Name",
        "    MediumSmooth,            !- Roughness",
        "    0.0127,                  !- Thickness {m}",
        "    0.160158849,             !- Conductivity {W/m-K}",
        "    800.923168698,           !- Density {kg/m3}",
        "    1087.84,                 !- Specific Heat {J/kg-K}",
        "    0.9,                     !- Thermal Absorptance",
        "    0.9,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    Gypsum Board Wall,       !- Name",
        "    MediumSmooth,            !- Roughness",
        "    0.0127,                  !- Thickness {m}",
        "    0.160158849,             !- Conductivity {W/m-K}",
        "    800.923168698,           !- Density {kg/m3}",
        "    1087.84,                 !- Specific Heat {J/kg-K}",
        "    1e-6,                    !- Thermal Absorptance",
        "    1e-6,                    !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    R-19 Insulation,         !- Name",
        "    Rough,                   !- Roughness",
        "    0.88871384,              !- Thickness {m}",
        "    0.25745056,              !- Conductivity {W/m-K}",
        "    3.05091836,              !- Density {kg/m3}",
        "    794.96,                  !- Specific Heat {J/kg-K}",
        "    0.9,                     !- Thermal Absorptance",
        "    0.9,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    R-A Lot,                 !- Name",
        "    Rough,                   !- Roughness",
        "    1.25,                    !- Thickness {m}",
        "    0.001,                   !- Conductivity {W/m-K}",
        "    3.05091836,              !- Density {kg/m3}",
        "    794.96,                  !- Specific Heat {J/kg-K}",
        "    0.9,                     !- Thermal Absorptance",
        "    0.9,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    Shingles,                !- Name",
        "    Rough,                   !- Roughness",
        "    0.006348984,             !- Thickness {m}",
        "    0.081932979,             !- Conductivity {W/m-K}",
        "    1121.292436177,          !- Density {kg/m3}",
        "    1256.04,                 !- Specific Heat {J/kg-K}",
        "    0.9,                     !- Thermal Absorptance",
        "    0.9,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    Felt,                    !- Name",
        "    Rough,                   !- Roughness",
        "    0.00216408,              !- Thickness {m}",
        "    0.081932979,             !- Conductivity {W/m-K}",
        "    1121.292436177,          !- Density {kg/m3}",
        "    1507.248,                !- Specific Heat {J/kg-K}",
        "    0.9,                     !- Thermal Absorptance",
        "    0.9,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    Plywood,                 !- Name",
        "    Rough,                   !- Roughness",
        "    0.012701016,             !- Thickness {m}",
        "    0.11544,                 !- Conductivity {W/m-K}",
        "    544.627754714,           !- Density {kg/m3}",
        "    1214.172,                !- Specific Heat {J/kg-K}",
        "    0.9,                     !- Thermal Absorptance",
        "    0.9,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    Hardboard Siding-Gable,  !- Name",
        "    MediumSmooth,            !- Roughness",
        "    0.0111125,               !- Thickness {m}",
        "    0.214957246,             !- Conductivity {W/m-K}",
        "    640.736,                 !- Density {kg/m3}",
        "    1172.304,                !- Specific Heat {J/kg-K}",
        "    0.90,                    !- Thermal Absorptance",
        "    0.7,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    Studs,                   !- Name",
        "    Rough,                   !- Roughness",
        "    0.0003137,               !- Thickness {m}",
        "    0.02189835,              !- Conductivity {W/m-K}",
        "    448.516974471,           !- Density {kg/m3}",
        "    1632.852,                !- Specific Heat {J/kg-K}",
        "    0.90,                    !- Thermal Absorptance",
        "    0.7,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    Hardboard Siding-Eave,   !- Name",
        "    MediumSmooth,            !- Roughness",
        "    0.0111125,               !- Thickness {m}",
        "    0.214957246,             !- Conductivity {W/m-K}",
        "    640.736,                 !- Density {kg/m3}",
        "    1172.304,                !- Specific Heat {J/kg-K}",
        "    0.90,                    !- Thermal Absorptance",
        "    0.7,                     !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Material,",
        "    HF-C5,                   !- Name",
        "    MediumRough,             !- Roughness",
        "    0.1015000,               !- Thickness {m}",
        "    1.729600,                !- Conductivity {W/m-K}",
        "    2243.000,                !- Density {kg/m3}",
        "    837.0000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    1.0;                     !- Visible Absorptance",

        "  Construction,",
        "    CeilingConstruction,     !- Name",
        "    R-19 Insulation,         !- Outside Layer",
        "    Gypsum Board;            !- Layer 2",

        "  Construction,",
        "    Reverse:CeilingConstruction,  !- Name",
        "    Gypsum Board,            !- Outside Layer",
        "    R-19 Insulation;         !- Layer 2",

        "  Construction,",
        "    Roof,                    !- Name",
        "    Shingles,                !- Outside Layer",
        "    Felt,                    !- Layer 2",
        "    Plywood;                 !- Layer 3",

        "  Construction,",
        "    Gables,                  !- Name",
        "    Hardboard Siding-Eave;   !- Outside Layer",

        "  Construction,",
        "    Eave Walls,              !- Name",
        "    Hardboard Siding-Eave;   !- Outside Layer",

        "  Construction,",
        "    Walls,                   !- Name",
        "    Hardboard Siding-Eave,   !- Outside Layer",
        "    R-A Lot,                 !- Layer 2",
        "    Gypsum Board Wall;       !- Layer 3",

        "  Construction,",
        "    LTFLOOR,                 !- Name",
        "    HF-C5;                   !- Outside Layer",

        "  GlobalGeometryRules,",
        "    UpperLeftCorner,         !- Starting Vertex Position",
        "    Counterclockwise,        !- Vertex Entry Direction",
        "    World;                   !- Coordinate System",

        "  Zone,",
        "    OCCUPIED ZONE,           !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    ATTIC ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  BuildingSurface:Detailed,",
        "    North Wall,              !- Name",
        "    Wall,                    !- Surface Type",
        "    Walls,                   !- Construction Name",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    16.764,                  !- Vertex 1 X-coordinate {m}",
        "    8.5344,                  !- Vertex 1 Y-coordinate {m}",
        "    2.70,                    !- Vertex 1 Z-coordinate {m}",
        "    16.764,                  !- Vertex 2 X-coordinate {m}",
        "    8.5344,                  !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    8.5344,                  !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    8.5344,                  !- Vertex 4 Y-coordinate {m}",
        "    2.70;                    !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    North Wall Attic,        !- Name",
        "    Wall,                    !- Surface Type",
        "    Eave Walls,              !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    16.764,                  !- Vertex 1 X-coordinate {m}",
        "    8.5344,                  !- Vertex 1 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 1 Z-coordinate {m}",
        "    16.764,                  !- Vertex 2 X-coordinate {m}",
        "    8.5344,                  !- Vertex 2 Y-coordinate {m}",
        "    2.70,                    !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    8.5344,                  !- Vertex 3 Y-coordinate {m}",
        "    2.70,                    !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    8.5344,                  !- Vertex 4 Y-coordinate {m}",
        "    2.7254;                  !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    East Wall,               !- Name",
        "    Wall,                    !- Surface Type",
        "    Walls,                   !- Construction Name",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    16.764,                  !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    2.70,                    !- Vertex 1 Z-coordinate {m}",
        "    16.764,                  !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    16.764,                  !- Vertex 3 X-coordinate {m}",
        "    8.5344,                  !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    16.764,                  !- Vertex 4 X-coordinate {m}",
        "    8.5344,                  !- Vertex 4 Y-coordinate {m}",
        "    2.70;                    !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    East Wall Attic,         !- Name",
        "    Wall,                    !- Surface Type",
        "    Gables,              !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    5,                       !- Number of Vertices",
        "    16.764,                  !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 1 Z-coordinate {m}",
        "    16.764,                  !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    2.70,                    !- Vertex 2 Z-coordinate {m}",
        "    16.764,                  !- Vertex 3 X-coordinate {m}",
        "    8.5344,                  !- Vertex 3 Y-coordinate {m}",
        "    2.70,                    !- Vertex 3 Z-coordinate {m}",
        "    16.764,                  !- Vertex 4 X-coordinate {m}",
        "    8.5344,                  !- Vertex 4 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 4 Z-coordinate {m}",
        "    16.764,                  !- Vertex 5 X-coordinate {m}",
        "    4.2672,                  !- Vertex 5 Y-coordinate {m}",
        "    4.5034;                  !- Vertex 5 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    South Wall,              !- Name",
        "    Wall,                    !- Surface Type",
        "    Walls,                   !- Construction Name",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    2.70,                    !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    16.764,                  !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    16.764,                  !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    2.70;                    !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    South Wall Attic,        !- Name",
        "    Wall,                    !- Surface Type",
        "    Eave Walls,              !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    2.70,                    !- Vertex 2 Z-coordinate {m}",
        "    16.764,                  !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    2.70,                    !- Vertex 3 Z-coordinate {m}",
        "    16.764,                  !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    2.7254;                  !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    West Wall,               !- Name",
        "    Wall,                    !- Surface Type",
        "    Walls,                   !- Construction Name",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    8.5344,                  !- Vertex 1 Y-coordinate {m}",
        "    2.70,                    !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    8.5344,                  !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    2.70;                    !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    West Wall Attic,         !- Name",
        "    Wall,                    !- Surface Type",
        "    Gables,              !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.50,                    !- View Factor to Ground",
        "    5,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    8.5344,                  !- Vertex 1 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    8.5344,                  !- Vertex 2 Y-coordinate {m}",
        "    2.70,                    !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    2.70,                    !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 4 Z-coordinate {m}",
        "    0,                       !- Vertex 5 X-coordinate {m}",
        "    4.2672,                  !- Vertex 5 Y-coordinate {m}",
        "    4.5034;                  !- Vertex 5 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    Zone Floor,              !- Name",
        "    Floor,                   !- Surface Type",
        "    LTFLOOR,                 !- Construction Name",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    ,                        !- Space Name",
        "    Ground,                  !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    0,                       !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    8.5344,                  !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    16.764,                  !- Vertex 3 X-coordinate {m}",
        "    8.5344,                  !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    16.764,                  !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    0;                       !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    Zone Ceiling,            !- Name",
        "    Ceiling,                 !- Surface Type",
        "    CeilingConstruction,     !- Construction Name",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Attic Floor,             !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    8.5344,                  !- Vertex 1 Y-coordinate {m}",
        "    2.70,                    !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    2.70,                    !- Vertex 2 Z-coordinate {m}",
        "    16.764,                  !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    2.70,                    !- Vertex 3 Z-coordinate {m}",
        "    16.764,                  !- Vertex 4 X-coordinate {m}",
        "    8.5344,                  !- Vertex 4 Y-coordinate {m}",
        "    2.70;                    !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    Attic Floor,             !- Name",
        "    Floor,                   !- Surface Type",
        "    Reverse:CeilingConstruction,  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zone Ceiling,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    16.764,                  !- Vertex 1 X-coordinate {m}",
        "    8.5344,                  !- Vertex 1 Y-coordinate {m}",
        "    2.70,                    !- Vertex 1 Z-coordinate {m}",
        "    16.764,                  !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    2.70,                    !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    2.70,                    !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    8.5344,                  !- Vertex 4 Y-coordinate {m}",
        "    2.70;                    !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    Attic Roof South,        !- Name",
        "    Roof,                    !- Surface Type",
        "    Roof,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    4.2672,                  !- Vertex 1 Y-coordinate {m}",
        "    4.5034,                  !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 2 Z-coordinate {m}",
        "    16.764,                  !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 3 Z-coordinate {m}",
        "    16.764,                  !- Vertex 4 X-coordinate {m}",
        "    4.2672,                  !- Vertex 4 Y-coordinate {m}",
        "    4.5034;                  !- Vertex 4 Z-coordinate {m}",

        "  BuildingSurface:Detailed,",
        "    Attic Roof North,        !- Name",
        "    Roof,                    !- Surface Type",
        "    Roof,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    16.764,                  !- Vertex 1 X-coordinate {m}",
        "    4.2672,                  !- Vertex 1 Y-coordinate {m}",
        "    4.5034,                  !- Vertex 1 Z-coordinate {m}",
        "    16.764,                  !- Vertex 2 X-coordinate {m}",
        "    8.5344,                  !- Vertex 2 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    8.5344,                  !- Vertex 3 Y-coordinate {m}",
        "    2.7254,                  !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    4.2672,                  !- Vertex 4 Y-coordinate {m}",
        "    4.5034;                  !- Vertex 4 Z-coordinate {m}",

        "  ZoneProperty:UserViewFactors:BySurfaceName,",
        "    ATTIC ZONE,              !- Zone Name",
        "    Attic Floor,		!=From Surface 1",
        "    Attic Floor,		!=To Surface 1",
        "    0.000000,",
        "    Attic Floor,		!=From Surface 1",
        "    Attic Roof South,		!=To Surface 2",
        "    0.476288,",
        "    Attic Floor,		!=From Surface 1",
        "    Attic Roof North,		!=To Surface 3",
        "    0.476288,",
        "    Attic Floor,		!=From Surface 1",
        "    East Wall Attic,		!=To Surface 4",
        "    0.023712,",
        "    Attic Floor,		!=From Surface 1",
        "    West Wall Attic,		!=To Surface 5",
        "    0.023712,",
        "    Attic Floor,		!=From Surface 1",
        "    North Wall Attic,		!=To Surface 6",
        "    0.000000,",
        "    Attic Floor,		!=From Surface 1",
        "    South Wall Attic,		!=To Surface 7",
        "    0.000000,",
        "    Attic Roof South,		!=From Surface 2",
        "    Attic Floor,		!=To Surface 1",
        "    0.879300,",
        "    Attic Roof South,		!=From Surface 2",
        "    Attic Roof South,		!=To Surface 2",
        "    0.000000,",
        "    Attic Roof South,		!=From Surface 2",
        "    Attic Roof North,		!=To Surface 3",
        "    0.067378,",
        "    Attic Roof South,		!=From Surface 2",
        "    East Wall Attic,		!=To Surface 4",
        "    0.026661,",
        "    Attic Roof South,		!=From Surface 2",
        "    West Wall Attic,		!=To Surface 5",
        "    0.026661,",
        "    Attic Roof South,		!=From Surface 2",
        "    North Wall Attic,		!=To Surface 6",
        "    0.000000,",
        "    Attic Roof South,		!=From Surface 2",
        "    South Wall Attic,		!=To Surface 7",
        "    0.000000,",
        "    Attic Roof North,		!=From Surface 3",
        "    Attic Floor,		!=To Surface 1",
        "    0.879300,",
        "    Attic Roof North,		!=From Surface 3",
        "    Attic Roof South,		!=To Surface 2",
        "    0.067378,",
        "    Attic Roof North,		!=From Surface 3",
        "    Attic Roof North,		!=To Surface 3",
        "    0.000000,",
        "    Attic Roof North,		!=From Surface 3",
        "    East Wall Attic,		!=To Surface 4",
        "    0.026661,",
        "    Attic Roof North,		!=From Surface 3",
        "    West Wall Attic,		!=To Surface 5",
        "    0.026661,",
        "    Attic Roof North,		!=From Surface 3",
        "    North Wall Attic,		!=To Surface 6",
        "    0.000000,",
        "    Attic Roof North,		!=From Surface 3",
        "    South Wall Attic,		!=To Surface 7",
        "    0.000000,",
        "    East Wall Attic,		!=From Surface 4",
        "    Attic Floor,		!=To Surface 1",
        "    0.447134,",
        "    East Wall Attic,		!=From Surface 4",
        "    Attic Roof South,		!=To Surface 2",
        "    0.272318,",
        "    East Wall Attic,		!=From Surface 4",
        "    Attic Roof North,		!=To Surface 3",
        "    0.272318,",
        "    East Wall Attic,		!=From Surface 4",
        "    East Wall Attic,		!=To Surface 4",
        "    0.000000,",
        "    East Wall Attic,		!=From Surface 4",
        "    West Wall Attic,		!=To Surface 5",
        "    0.008231,",
        "    East Wall Attic,		!=From Surface 4",
        "    North Wall Attic,		!=To Surface 6",
        "    0.000000,",
        "    East Wall Attic,		!=From Surface 4",
        "    South Wall Attic,		!=To Surface 7",
        "    0.000000,",
        "    West Wall Attic,		!=From Surface 5",
        "    Attic Floor,		!=To Surface 1",
        "    0.447134,",
        "    West Wall Attic,		!=From Surface 5",
        "    Attic Roof South,		!=To Surface 2",
        "    0.272318,",
        "    West Wall Attic,		!=From Surface 5",
        "    Attic Roof North,		!=To Surface 3",
        "    0.272318,",
        "    West Wall Attic,		!=From Surface 5",
        "    East Wall Attic,		!=To Surface 4",
        "    0.008231,",
        "    West Wall Attic,		!=From Surface 5",
        "    West Wall Attic,		!=To Surface 5",
        "    0.000000,",
        "    West Wall Attic,		!=From Surface 5",
        "    North Wall Attic,		!=To Surface 6",
        "    0.000000,",
        "    West Wall Attic,		!=From Surface 5",
        "    South Wall Attic,		!=To Surface 7",
        "    0.000000,",
        "    North Wall Attic,		!=From Surface 6",
        "    Attic Floor,		!=To Surface 1",
        "    0.000000,",
        "    North Wall Attic,		!=From Surface 6",
        "    Attic Roof South,		!=To Surface 2",
        "    0.000000,",
        "    North Wall Attic,		!=From Surface 6",
        "    Attic Roof North,		!=To Surface 3",
        "    0.000000,",
        "    North Wall Attic,		!=From Surface 6",
        "    East Wall Attic,		!=To Surface 4",
        "    0.000000,",
        "    North Wall Attic,		!=From Surface 6",
        "    West Wall Attic,		!=To Surface 5",
        "    0.000000,",
        "    North Wall Attic,		!=From Surface 6",
        "    North Wall Attic,		!=To Surface 6",
        "    1.000000,",
        "    North Wall Attic,		!=From Surface 6",
        "    South Wall Attic,		!=To Surface 7",
        "    0.000000,",
        "    South Wall Attic,		!=From Surface 7",
        "    Attic Floor,		!=To Surface 1",
        "    0.000000,",
        "    South Wall Attic,		!=From Surface 7",
        "    Attic Roof South,		!=To Surface 2",
        "    0.000000,",
        "    South Wall Attic,		!=From Surface 7",
        "    Attic Roof North,		!=To Surface 3",
        "    0.000000,",
        "    South Wall Attic,		!=From Surface 7",
        "    East Wall Attic,		!=To Surface 4",
        "    0.000000,",
        "    South Wall Attic,		!=From Surface 7",
        "    West Wall Attic,		!=To Surface 5",
        "    0.000000,",
        "    South Wall Attic,		!=From Surface 7",
        "    North Wall Attic,		!=To Surface 6",
        "    0.000000,",
        "    South Wall Attic,		!=From Surface 7",
        "    South Wall Attic,		!=To Surface 7",
        "    1.000000;",

        "  AirflowNetwork:SimulationControl,",
        "    House AirflowNetwork,    !- Name",
        "    MultizoneWithDistribution,  !- AirflowNetwork Control",
        "    SurfaceAverageCalculation,  !- Wind Pressure Coefficient Type",
        "    ,                        !- Height Selection for Local Wind Pressure Calculation",
        "    LOWRISE,                 !- Building Type",
        "    500,                     !- Maximum Number of Iterations {dimensionless}",
        "    ZeroNodePressures,       !- Initialization Type",
        "    1.0E-04,                 !- Relative Airflow Convergence Tolerance {dimensionless}",
        "    1.0E-06,                 !- Absolute Airflow Convergence Tolerance {kg/s}",
        "    -0.5,                    !- Convergence Acceleration Limit {dimensionless}",
        "    0.0,                     !- Azimuth Angle of Long Axis of Building {deg}",
        "    1.0;                     !- Ratio of Building Width Along Short Axis to Width Along Long Axis",

        "  AirflowNetwork:MultiZone:Zone,",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    NOVENT,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    ATTIC ZONE,              !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    North Wall Attic,        !- Surface Name",
        "    NorthEaveLeak,           !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    South Wall Attic,        !- Surface Name",
        "    SouthEaveLeak,           !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    East Wall,               !- Surface Name",
        "    EastLeak,                !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    West Wall,               !- Surface Name",
        "    WestLeak,                !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "    ReferenceCrackConditions,!- Name",
        "    20.0,                    !- Reference Temperature {C}",
        "    101325,                  !- Reference Barometric Pressure {Pa}",
        "    0.0;                     !- Reference Humidity Ratio {kgWater/kgDryAir}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    NorthEaveLeak,           !- Name",
        "    0.2,                     !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.65,                    !- Air Mass Flow Exponent {dimensionless}",
        "    ReferenceCrackConditions;!- Reference Crack Conditions",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    SouthEaveLeak,           !- Name",
        "    0.2,                     !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.65,                    !- Air Mass Flow Exponent {dimensionless}",
        "    ReferenceCrackConditions;!- Reference Crack Conditions",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    EastLeak,                !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.65,                    !- Air Mass Flow Exponent {dimensionless}",
        "    ReferenceCrackConditions;!- Reference Crack Conditions",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    WestLeak,                !- Name",
        "    0.03,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.65,                    !- Air Mass Flow Exponent {dimensionless}",
        "    ReferenceCrackConditions;!- Reference Crack Conditions",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentInletNode,      !- Name",
        "    Zone Equipment Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    SplitterNode,            !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneSupplyNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneSupplyRegisterNode,  !- Name",
        "    Zone Inlet Node,         !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneOutletNode,          !- Name",
        "    Zone Outlet Node,        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneReturnNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MixerNode,               !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainReturnNode,          !- Name",
        "    Return Air Mixer Outlet, !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainInletNode,           !- Name",
        "    Air Loop Inlet Node,     !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanInletNode,            !- Name",
        "    Air Loop Inlet Node,     !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanOutletNode,           !- Name",
        "    Cooling Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingInletNode,        !- Name",
        "    Heating Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingOutletNode,       !- Name",
        "    Air Loop Outlet Node,    !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTrunk,               !- Name",
        "    2.0,                     !- Duct Length {m}",
        "    0.4064,                  !- Hydraulic Diameter {m}",
        "    0.1297,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.7139,                  !- Heat Transmittance Coefficient (U-Factor) for Duct Construction {W/m2-K}",
        "    0.0000001;               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneSupply,              !- Name",
        "    16.76,                   !- Duct Length {m}",
        "    0.3048,                  !- Hydraulic Diameter {m}",
        "    0.073205,                !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.91,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.00613207547169811,     !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.0325,                  !- Outside Convection Coefficient {W/m2-K}",
        "    0.1625;                  !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneReturn,              !- Name",
        "    3.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.00122641509433962,     !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.0065,                  !- Outside Convection Coefficient {W/m2-K}",
        "    0.0325;                  !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneConnectionDuct,      !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    0.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.00122641509433962,     !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.0065,                  !- Outside Convection Coefficient {W/m2-K}",
        "    0.0325;                  !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainReturn,              !- Name",
        "    1.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.00122641509433962,     !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.0065,                  !- Outside Convection Coefficient {W/m2-K}",
        "    0.0325;                  !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopReturn,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.00,                    !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    0.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.00122641509433962,     !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.0065,                  !- Outside Convection Coefficient {W/m2-K}",
        "    0.0325;                  !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopSupply,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.00,                    !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    0.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.00122641509433962,     !- Overall Heat Transmittance Coefficient (U-Factor) from Air to Air {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.0065,                  !- Outside Convection Coefficient {W/m2-K}",
        "    0.0325;                  !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Fan,",
        "    Supply Fan 1,            !- Fan Name",
        "    Fan:ConstantVolume;      !- Supply Fan Object Type",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    ACDXCoil 1,              !- Coil Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.000;                   !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Main Heating Coil 1,     !- Coil Name",
        "    Coil:Heating:Fuel,       !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.000;                   !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:DuctViewFactors,",
        "    ZoneSupplyLink1,         !- Name of Linkage",
        "    1.0,                     !- Surface Exposure Fraction",
        "    0.9,                     !- Duct surface emittance",
        "    Attic Floor,             !- Surface 1",
        "    0.483577,                !- View Factor for Surface 1",
        "    Attic Roof North,        !- Surface 2",
        "    0.237692,                !- View Factor for Surface 2",
        "    Attic Roof South,        !- Surface 3",
        "    0.237692,                !- View Factor for Surface 3",
        "    East Wall Attic,         !- Surface 4",
        "    0.02052,                 !- View Factor for Surface 4",
        "    West Wall Attic,         !- Surface 5",
        "    0.02052;                 !- View Factor for Surface 5",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link,               !- Name",
        "    EquipmentInletNode,      !- Node 1 Name",
        "    SplitterNode,            !- Node 2 Name",
        "    MainTrunk,               !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneSupplyLink1,         !- Name",
        "    SplitterNode,            !- Node 1 Name",
        "    ZoneSupplyNode,          !- Node 2 Name",
        "    ZoneSupply,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneSupply1Link2,        !- Name",
        "    ZoneSupplyNode,          !- Node 1 Name",
        "    ZoneSupplyRegisterNode,  !- Node 2 Name",
        "    ZoneSupply,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneSupplyConnectionLink,!- Name",
        "    ZoneSupplyRegisterNode,  !- Node 1 Name",
        "    OCCUPIED ZONE,           !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneReturnConnectionLink,!- Name",
        "    OCCUPIED ZONE,           !- Node 1 Name",
        "    ZoneOutletNode,          !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneReturn1Link,         !- Name",
        "    ZoneOutletNode,          !- Node 1 Name",
        "    ZoneReturnNode,          !- Node 2 Name",
        "    ZoneReturn,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneReturn2Link,         !- Name",
        "    ZoneReturnNode,          !- Node 1 Name",
        "    MixerNode,               !- Node 2 Name",
        "    ZoneReturn,              !- Component Name",
        "    OCCUPIED Zone;           !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ReturnMixerLink,         !- Name",
        "    MixerNode,               !- Node 1 Name",
        "    MainReturnNode,          !- Node 2 Name",
        "    MainReturn,              !- Component Name",
        "    OCCUPIED Zone;           !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SystemReturnLink,        !- Name",
        "    MainReturnNode,          !- Node 1 Name",
        "    MainInletNode,           !- Node 2 Name",
        "    AirLoopReturn;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SystemInletLink,         !- Name",
        "    MainInletNode,           !- Node 1 Name",
        "    FanInletNode,            !- Node 2 Name",
        "    MainReturn,              !- Component Name",
        "    OCCUPIED ZONE;           !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SupplyFanLink,           !- Name",
        "    FanInletNode,            !- Node 1 Name",
        "    FanOutletNode,           !- Node 2 Name",
        "    Supply Fan 1;            !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    CoolingCoilLink,         !- Name",
        "    FanOutletNode,           !- Node 1 Name",
        "    HeatingInletNode,        !- Node 2 Name",
        "    ACDXCoil 1;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    HeatingCoilLink,         !- Name",
        "    HeatingInletNode,        !- Node 1 Name",
        "    HeatingOutletNode,       !- Node 2 Name",
        "    Main Heating Coil 1;     !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    EquipmentAirLoopLink,    !- Name",
        "    HeatingOutletNode,       !- Node 1 Name",
        "    EquipmentInletNode,      !- Node 2 Name",
        "    AirLoopSupply;           !- Component Name",

        "  ZoneControl:Thermostat,",
        "    Zone Thermostat,         !- Name",
        "    OCCUPIED ZONE,           !- Zone or ZoneList Name",
        "    Dual Zone Control Type Sched,  !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    Setpoints;               !- Control 1 Name",

        "  ThermostatSetpoint:DualSetpoint,",
        "    Setpoints,               !- Name",
        "    Dual Heating Setpoints,  !- Heating Setpoint Temperature Schedule Name",
        "    Dual Cooling Setpoints;  !- Cooling Setpoint Temperature Schedule Name",

        "  ZoneHVAC:AirDistributionUnit,",
        "    ZoneDirectAirADU,        !- Name",
        "    Zone1NoReheatAirOutletNode,  !- Air Distribution Unit Outlet Node Name",
        "    AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "    ZoneDirectAir;           !- Air Terminal Name",

        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "    ZoneDirectAir,           !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    Zone Inlet Node 2AT,     !- Air Inlet Node Name",
        "    Zone Inlet Node,         !- Zone Supply Air Node Name",
        "    2.36;                    !- Maximum Air Flow Rate {m3/s}",

        "  ZoneHVAC:EquipmentList,",
        "    ZoneEquipment,           !- Name",
        "    SequentialLoad,          !- Load Distribution Scheme",
        "    ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "    ZoneDirectAirADU,        !- Zone Equipment 1 Name",
        "    1,                       !- Zone Equipment 1 Cooling Sequence",
        "    1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "    ,                        !- Zone Equipment 1 Sequential Cooling Fraction",
        "    ;                        !- Zone Equipment 1 Sequential Heating Fraction",

        "  ZoneHVAC:EquipmentConnections,",
        "    OCCUPIED ZONE,           !- Zone Name",
        "    ZoneEquipment,           !- Zone Conditioning Equipment List Name",
        "    ZoneInlets,              !- Zone Air Inlet Node or NodeList Name",
        "    ,                        !- Zone Air Exhaust Node or NodeList Name",
        "    Zone Node,               !- Zone Air Node Name",
        "    Zone Outlet Node;        !- Zone Return Air Node Name",

        "  Fan:ConstantVolume,",
        "    Supply Fan 1,            !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    0.7,                     !- Fan Total Efficiency",
        "    400.0,                   !- Pressure Rise {Pa}",
        "    2.36,                    !- Maximum Flow Rate {m3/s}",
        "    0.9,                     !- Motor Efficiency",
        "    1.0,                     !- Motor In Airstream Fraction",
        "    Air Loop Inlet Node,     !- Air Inlet Node Name",
        "    Cooling Coil Air Inlet Node;  !- Air Outlet Node Name",

        "  Coil:Cooling:DX:SingleSpeed,",
        "    ACDXCoil 1,              !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    21000,                   !- Gross Rated Total Cooling Capacity {W}",
        "    0.8,                     !- Gross Rated Sensible Heat Ratio",
        "    3.0,                     !- Gross Rated Cooling COP {W/W}",
        "    2.36,                    !- Rated Air Flow Rate {m3/s}",
        "    ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    Cooling Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- Air Outlet Node Name",
        "    WindACCoolCapFT,         !- Total Cooling Capacity Function of Temperature Curve Name",
        "    WindACCoolCapFFF,        !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "    WindACEIRFT,             !- Energy Input Ratio Function of Temperature Curve Name",
        "    WindACEIRFFF,            !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "    WindACPLFFPLR;           !- Part Load Fraction Correlation Curve Name",

        "  Coil:Heating:Fuel,",
        "    Main Heating Coil 1,     !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    NaturalGas,              !- Fuel Type",
        "    0.8,                     !- Burner Efficiency",
        "    8000000,                 !- Nominal Capacity {W}",
        "    Heating Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Air Loop Outlet Node,    !- Air Outlet Node Name",
        "    Air Loop Outlet Node;    !- Temperature Setpoint Node Name",

        "  CoilSystem:Cooling:DX,",
        "    DX Cooling Coil System 1,!- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    Cooling Coil Air Inlet Node,  !- DX Cooling Coil System Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- DX Cooling Coil System Outlet Node Name",
        "    Heating Coil Air Inlet Node,  !- DX Cooling Coil System Sensor Node Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "    ACDXCoil 1;              !- Cooling Coil Name",

        "  AirLoopHVAC,",
        "    Typical Residential System,  !- Name",
        "    ,                        !- Controller List Name",
        "    Reheat System 1 Avail List,  !- Availability Manager List Name",
        "    2.36,                    !- Design Supply Air Flow Rate {m3/s}",
        "    Air Loop Branches,       !- Branch List Name",
        "    ,                        !- Connector List Name",
        "    Air Loop Inlet Node,     !- Supply Side Inlet Node Name",
        "    Return Air Mixer Outlet, !- Demand Side Outlet Node Name",
        "    Zone Equipment Inlet Node,  !- Demand Side Inlet Node Names",
        "    Air Loop Outlet Node;    !- Supply Side Outlet Node Names",

        "  AirLoopHVAC:ZoneSplitter,",
        "    Zone Supply Air Splitter,!- Name",
        "    Zone Equipment Inlet Node,  !- Inlet Node Name",
        "    Zone Inlet Node 2AT;      !- Outlet 1 Node Name",

        "  AirLoopHVAC:SupplyPath,",
        "    TermReheatSupplyPath,    !- Name",
        "    Zone Equipment Inlet Node,  !- Supply Air Path Inlet Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component 1 Object Type",
        "    Zone Supply Air Splitter;!- Component 1 Name",

        "  AirLoopHVAC:ZoneMixer,",
        "    Zone Return Air Mixer,   !- Name",
        "    Return Air Mixer Outlet, !- Outlet Node Name",
        "    Zone Outlet Node;        !- Inlet 1 Node Name",

        "  AirLoopHVAC:ReturnPath,",
        "    TermReheatReturnPath,    !- Name",
        "    Return Air Mixer Outlet, !- Return Air Path Outlet Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component 1 Object Type",
        "    Zone Return Air Mixer;   !- Component 1 Name",

        "  Branch,",
        "    Air Loop Main Branch,    !- Name",
        "    ,                        !- Pressure Drop Curve Name",
        "    Fan:ConstantVolume,      !- Component 1 Object Type",
        "    Supply Fan 1,            !- Component 1 Name",
        "    Air Loop Inlet Node,     !- Component 1 Inlet Node Name",
        "    Cooling Coil Air Inlet Node,  !- Component 1 Outlet Node Name",
        "    CoilSystem:Cooling:DX,   !- Component 2 Object Type",
        "    DX Cooling Coil System 1,!- Component 2 Name",
        "    Cooling Coil Air Inlet Node,  !- Component 2 Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- Component 2 Outlet Node Name",
        "    Coil:Heating:Fuel,       !- Component 3 Object Type",
        "    Main Heating Coil 1,     !- Component 3 Name",
        "    Heating Coil Air Inlet Node,  !- Component 3 Inlet Node Name",
        "    Air Loop Outlet Node;    !- Component 3 Outlet Node Name",

        "  BranchList,",
        "    Air Loop Branches,       !- Name",
        "    Air Loop Main Branch;    !- Branch 1 Name",

        "  NodeList,",
        "    ZoneInlets,              !- Name",
        "    Zone Inlet Node;         !- Node 1 Name",

        "  NodeList,",
        "    Supply Air Temp Nodes,   !- Name",
        "    Heating Coil Air Inlet Node,  !- Node 1 Name",
        "    Air Loop Outlet Node;    !- Node 2 Name",

    });

    ASSERT_TRUE(process_idf(idf_objects));

    // Read objects
    HeatBalanceManager::GetHeatBalanceInput(*state);
    HeatBalanceManager::AllocateHeatBalArrays(*state);
    state->dataEnvrn->OutBaroPress = 101000;
    state->dataHVACGlobal->TimeStepSys = state->dataGlobal->TimeStepZone;
    state->dataHVACGlobal->TimeStepSysSec = state->dataHVACGlobal->TimeStepSys * Constant::SecInHour;

    // Read AirflowNetwork inputs
    ASSERT_THROW(state->afn->get_input(), std::runtime_error);

    std::string const error_string = delimited_string({
        "   ** Warning ** GetHTSurfaceData: Surfaces with interface to Ground found but no \"Ground Temperatures\" were input.",
        "   **   ~~~   ** Found first in surface=ZONE FLOOR",
        "   **   ~~~   ** Defaults, constant throughout the year of (0.0) will be used.",
        "   ** Severe  ** AirflowNetwork::Solver::get_input: AirflowNetwork:Distribution:Node=\"FANINLETNODE\" Duplicated Component Name or Node "
        "Name=\"AIR "
        "LOOP INLET NODE\". Please make a correction.",
        "   **  Fatal  ** AirflowNetwork::Solver::get_input: Errors found getting inputs. Previous error(s) cause program termination.",
        "   ...Summary of Errors that led to program termination:",
        "   ..... Reference severe error count=1",
        "   ..... Last severe error=AirflowNetwork::Solver::get_input: AirflowNetwork:Distribution:Node=\"FANINLETNODE\" Duplicated Component Name "
        "or Node "
        "Name=\"AIR LOOP INLET NODE\". Please make a correction.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_SenLatLoadsConservation_Test)
{
    // Issue 7891
    Real64 T1 = 25.0;
    Real64 W1 = 0.01;
    Real64 T2 = 15.0;
    Real64 W2 = 0.005;
    Real64 hdiff = Psychrometrics::PsyHFnTdbW(T1, W1) - Psychrometrics::PsyHFnTdbW(T2, W2);
    Real64 sen = Psychrometrics::PsyCpAirFnW(W1) * (T1 - T2);
    Real64 lat = Psychrometrics::PsyHgAirFnWTdb(W2, T2) * (W1 - W2);
    Real64 sen1 = Psychrometrics::PsyCpAirFnW((W1 + W2) / 2.0) * (T1 - T2);
    Real64 lat1 = Psychrometrics::PsyHgAirFnWTdb(W2, (T2 + T1) / 2.0) * (W1 - W2);
    Real64 sum = sen + lat;
    // single value
    EXPECT_NEAR(hdiff, sum, 0.0001);
    // Average value
    sum = sen1 + lat1;
    EXPECT_NEAR(hdiff, sum, 0.0001);
}

TEST_F(EnergyPlusFixture, DISABLED_AirLoopNumTest)
{

    std::string const idf_objects = delimited_string({

        "  Building,",
        "    Small Office with AirflowNetwork model,  !- Name",
        "    0,                       !- North Axis {deg}",
        "    Suburbs,                 !- Terrain",
        "    0.001,                   !- Loads Convergence Tolerance Value",
        "    0.0050000,               !- Temperature Convergence Tolerance Value {deltaC}",
        "    FullInteriorAndExterior, !- Solar Distribution",
        "    25,                      !- Maximum Number of Warmup Days",
        "    6;                       !- Minimum Number of Warmup Days",

        "  Timestep,6;",

        "  SurfaceConvectionAlgorithm:Inside,TARP;",

        "  SurfaceConvectionAlgorithm:Outside,DOE-2;",

        "  HeatBalanceAlgorithm,ConductionTransferFunction;",

        "  Output:DebuggingData,0,0;",

        "  ZoneCapacitanceMultiplier:ResearchSpecial,",
        "    Multiplier,              !- Name",
        "    ,                        !- Zone or ZoneList Name",
        "    1.0,                     !- Temperature Capacity Multiplier",
        "    1.0,                     !- Humidity Capacity Multiplier",
        "    1.0,                     !- Carbon Dioxide Capacity Multiplier",
        "    ;                        !- Generic Contaminant Capacity Multiplier",

        "  SimulationControl,",
        "    No,                     !- Do Zone Sizing Calculation",
        "    No,                     !- Do System Sizing Calculation",
        "    No,                      !- Do Plant Sizing Calculation",
        "    Yes,                     !- Run Simulation for Sizing Periods",
        "    No;                      !- Run Simulation for Weather File Run Periods",

        "  Sizing:Parameters,",
        "    1.53,                    !- Heating Sizing Factor",
        "    1.70,                    !- Cooling Sizing Factor",
        "    6;                       !- Timesteps in Averaging Window",

        "  RunPeriod,",
        "    Spring run,              !- Name",
        "    4,                       !- Begin Month",
        "    1,                       !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    4,                       !- End Month",
        "    1,                       !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  RunPeriod,",
        "    Winter run,              !- Name",
        "    1,                       !- Begin Month",
        "    14,                      !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    1,                       !- End Month",
        "    14,                      !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  RunPeriod,",
        "    Summer run,              !- Name",
        "    7,                       !- Begin Month",
        "    7,                       !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    7,                       !- End Month",
        "    7,                       !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  Site:Location,",
        "    CHICAGO_IL_USA TMY2-94846,  !- Name",
        "    41.78,                   !- Latitude {deg}",
        "    -87.75,                  !- Longitude {deg}",
        "    -6.00,                   !- Time Zone {hr}",
        "    190.00;                  !- Elevation {m}",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Heating 99% Design Conditions DB,  !- Name",
        "    1,                       !- Month",
        "    21,                      !- Day of Month",
        "    WinterDesignDay,         !- Day Type",
        "    -17.3,                   !- Maximum Dry-Bulb Temperature {C}",
        "    0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    -17.3,                   !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    4.9,                     !- Wind Speed {m/s}",
        "    270,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    0.0;                     !- Sky Clearness",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Cooling 1% Design Conditions DB/MCWB,  !- Name",
        "    7,                       !- Month",
        "    21,                      !- Day of Month",
        "    SummerDesignDay,         !- Day Type",
        "    31.5,                    !- Maximum Dry-Bulb Temperature {C}",
        "    10.7,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    23.0,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    5.3,                     !- Wind Speed {m/s}",
        "    230,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    1.0;                     !- Sky Clearness",

        "  Material,",
        "    A1 - 1 IN STUCCO,        !- Name",
        "    Smooth,                  !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    0.6918309,               !- Conductivity {W/m-K}",
        "    1858.142,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Material,",
        "    C4 - 4 IN COMMON BRICK,  !- Name",
        "    Rough,                   !- Roughness",
        "    0.1014984,               !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1922.216,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7600000,               !- Solar Absorptance",
        "    0.7600000;               !- Visible Absorptance",

        "  Material,",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Name",
        "    Smooth,                  !- Roughness",
        "    1.9050000E-02,           !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1601.846,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Material,",
        "    C6 - 8 IN CLAY TILE,     !- Name",
        "    Smooth,                  !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    0.5707605,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.8200000,               !- Solar Absorptance",
        "    0.8200000;               !- Visible Absorptance",

        "  Material,",
        "    C10 - 8 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "  Material,",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Name",
        "    Rough,                   !- Roughness",
        "    1.2710161E-02,           !- Thickness {m}",
        "    1.435549,                !- Conductivity {W/m-K}",
        "    881.0155,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5500000,               !- Solar Absorptance",
        "    0.5500000;               !- Visible Absorptance",

        "  Material,",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Name",
        "    Rough,                   !- Roughness",
        "    9.5402403E-03,           !- Thickness {m}",
        "    0.1902535,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "  Material,",
        "    B5 - 1 IN DENSE INSULATION,  !- Name",
        "    VeryRough,               !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    4.3239430E-02,           !- Conductivity {W/m-K}",
        "    91.30524,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5000000,               !- Solar Absorptance",
        "    0.5000000;               !- Visible Absorptance",

        "  Material,",
        "    C12 - 2 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    5.0901599E-02,           !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "  Material,",
        "    1.375in-Solid-Core,      !- Name",
        "    Smooth,                  !- Roughness",
        "    3.4925E-02,              !- Thickness {m}",
        "    0.1525000,               !- Conductivity {W/m-K}",
        "    614.5000,                !- Density {kg/m3}",
        "    1630.0000,               !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  WindowMaterial:Glazing,",
        "    ELECTRO GLASS LIGHT STATE,  !- Name",
        "    SpectralAverage,         !- Optical Data Type",
        "    ,                        !- Window Glass Spectral Data Set Name",
        "    0.006,                   !- Thickness {m}",
        "    0.814,                   !- Solar Transmittance at Normal Incidence",
        "    0.086,                   !- Front Side Solar Reflectance at Normal Incidence",
        "    0.086,                   !- Back Side Solar Reflectance at Normal Incidence",
        "    0.847,                   !- Visible Transmittance at Normal Incidence",
        "    0.099,                   !- Front Side Visible Reflectance at Normal Incidence",
        "    0.099,                   !- Back Side Visible Reflectance at Normal Incidence",
        "    0.0,                     !- Infrared Transmittance at Normal Incidence",
        "    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
        "    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
        "    0.9;                     !- Conductivity {W/m-K}",

        "  WindowMaterial:Glazing,",
        "    ELECTRO GLASS DARK STATE,!- Name",
        "    SpectralAverage,         !- Optical Data Type",
        "    ,                        !- Window Glass Spectral Data Set Name",
        "    0.006,                   !- Thickness {m}",
        "    0.111,                   !- Solar Transmittance at Normal Incidence",
        "    0.179,                   !- Front Side Solar Reflectance at Normal Incidence",
        "    0.179,                   !- Back Side Solar Reflectance at Normal Incidence",
        "    0.128,                   !- Visible Transmittance at Normal Incidence",
        "    0.081,                   !- Front Side Visible Reflectance at Normal Incidence",
        "    0.081,                   !- Back Side Visible Reflectance at Normal Incidence",
        "    0.0,                     !- Infrared Transmittance at Normal Incidence",
        "    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
        "    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
        "    0.9;                     !- Conductivity {W/m-K}",

        "  WindowMaterial:Gas,",
        "    WinAirGap,               !- Name",
        "    AIR,                     !- Gas Type",
        "    0.013;                   !- Thickness {m}",

        "  Construction,",
        "    EXTWALL80,               !- Name",
        "    A1 - 1 IN STUCCO,        !- Outside Layer",
        "    C4 - 4 IN COMMON BRICK,  !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "  Construction,",
        "    PARTITION06,             !- Name",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "    C6 - 8 IN CLAY TILE,     !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "  Construction,",
        "    FLOOR SLAB 8 IN,         !- Name",
        "    C10 - 8 IN HW CONCRETE;  !- Outside Layer",

        "  Construction,",
        "    ROOF34,                  !- Name",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Outside Layer",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Layer 2",
        "    C12 - 2 IN HW CONCRETE;  !- Layer 3",

        "  Construction,",
        "    CEILING:ZONE,            !- Name",
        "    B5 - 1 IN DENSE INSULATION,  !- Outside Layer",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 2",

        "  Construction,",
        "    CEILING:ATTIC,           !- Name",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "    B5 - 1 IN DENSE INSULATION;  !- Layer 2",

        "  Construction,",
        "    ELECTRO-CON-LIGHT,       !- Name",
        "    ELECTRO GLASS LIGHT STATE,  !- Outside Layer",
        "    WinAirGap,               !- Layer 2",
        "    ELECTRO GLASS LIGHT STATE;  !- Layer 3",

        "  Construction,",
        "    ELECTRO-CON-DARK,        !- Name",
        "    ELECTRO GLASS DARK STATE, !- Outside Layer",
        "    WinAirGap,               !- Layer 2",
        "    ELECTRO GLASS DARK STATE; !- Layer 3",

        "  Construction,",
        "    DOOR-CON,                !- Name",
        "    1.375in-Solid-Core;      !- Outside Layer",

        "  Zone,",
        "    West Zone,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    EAST ZONE,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    NORTH ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    ATTIC ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    ATTIC NORTH ZONE,        !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  GlobalGeometryRules,",
        "    UpperLeftCorner,         !- Starting Vertex Position",
        "    CounterClockWise,        !- Vertex Entry Direction",
        "    World;                   !- Coordinate System",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn001:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    West Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn002:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    EAST ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Flr002,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Wall003,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn003:Ceil001,           !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:ZONE,            !- Construction Name",
        "    NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn005:Flr001,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,0,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,0,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall008,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,0,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall009,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC Zone,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn005:Wall004,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Wall010,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn005:Wall005,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Roof001,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Roof002,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,0,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,0,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn001:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn004:Flr002,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn002:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    12.19200,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    12.19200,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall001,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall002,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    9.144000,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall003,           !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall004,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Wall009,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Wall005,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn004:Wall010,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.9624,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.9624;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Flr001,            !- Name",
        "    Floor,                   !- Surface Type",
        "    CEILING:ATTIC,           !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Zn003:Ceil001,           !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,12.19200,3.048000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,12.19200,3.048000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Zn005:Roof001,           !- Name",
        "    Roof,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,12.19200,3.962400,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.096000,3.962400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    9.144000,6.096000,3.962400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    9.144000,12.19200,3.962400;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn001:Wall001:Win001,    !- Name",
        "    Window,                  !- Surface Type",
        "    ELECTRO-CON-LIGHT,       !- Construction Name",
        "    Zn001:Wall001,           !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,     !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    0.548000,0,2.5000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0.548000,0,0.5000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    5.548000,0,0.5000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    5.548000,0,2.5000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn001:Wall003:Door001,   !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Zn001:Wall003,           !- Building Surface Name",
        "    Zn003:Wall004:Door001,   !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    3.500,6.096000,2.0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    3.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    2.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    2.500,6.096000,2.0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn003:Wall002:Win001,    !- Name",
        "    Window,                  !- Surface Type",
        "    ELECTRO-CON-LIGHT,       !- Construction Name",
        "    Zn003:Wall002,           !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,     !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    5.548000,12.19200,2.5000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    5.548000,12.19200,0.5000,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0.548000,12.19200,0.5000,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0.548000,12.19200,2.5000;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    Zn003:Wall004:Door001,   !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Zn003:Wall004,           !- Building Surface Name",
        "    Zn001:Wall003:Door001,   !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    2.500,6.096000,2.0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    2.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    3.500,6.096000,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    3.500,6.096000,2.0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  ScheduleTypeLimits,",
        "    Any Number;              !- Name",

        "  ScheduleTypeLimits,",
        "    Fraction,                !- Name",
        "    0.0,                     !- Lower Limit Value",
        "    1.0,                     !- Upper Limit Value",
        "    CONTINUOUS;              !- Numeric Type",

        "  ScheduleTypeLimits,",
        "    Temperature,             !- Name",
        "    -60,                     !- Lower Limit Value",
        "    200,                     !- Upper Limit Value",
        "    CONTINUOUS,              !- Numeric Type",
        "    Temperature;             !- Unit Type",

        "  ScheduleTypeLimits,",
        "    Control Type,            !- Name",
        "    0,                       !- Lower Limit Value",
        "    4,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "  ScheduleTypeLimits,",
        "    On/Off,                  !- Name",
        "    0,                       !- Lower Limit Value",
        "    1,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "  Schedule:Compact,",
        "    WindowVentSched,         !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,25.55,      !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: AllDays,            !- Field 6",
        "    Until: 24:00,21.11,      !- Field 7",
        "    Through: 12/31,          !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until: 24:00,25.55;      !- Field 11",

        "  Schedule:Compact,",
        "    Activity Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,131.8;      !- Field 3",

        "  Schedule:Compact,",
        "    Work Eff Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.0;        !- Field 3",

        "  Schedule:Compact,",
        "    Clothing Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,1.0;        !- Field 3",

        "  Schedule:Compact,",
        "    Air Velo Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.137;      !- Field 3",

        "  Schedule:Compact,",
        "    OFFICE OCCUPANCY,        !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,0.0,         !- Field 3",
        "    Until: 7:00,0.10,        !- Field 5",
        "    Until: 8:00,0.50,        !- Field 7",
        "    Until: 12:00,1.00,       !- Field 9",
        "    Until: 13:00,0.50,       !- Field 11",
        "    Until: 16:00,1.00,       !- Field 13",
        "    Until: 17:00,0.50,       !- Field 15",
        "    Until: 18:00,0.10,       !- Field 17",
        "    Until: 24:00,0.0,        !- Field 19",
        "    For: AllOtherDays,       !- Field 21",
        "    Until: 24:00,0.0;        !- Field 22",

        "  Schedule:Compact,",
        "    INTERMITTENT,            !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 8:00,0.0,         !- Field 3",
        "    Until: 18:00,1.00,       !- Field 5",
        "    Until: 24:00,0.0,        !- Field 7",
        "    For: AllOtherDays,       !- Field 9",
        "    Until: 24:00,0.0;        !- Field 10",

        "  Schedule:Compact,",
        "    OFFICE LIGHTING,         !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,0.05,        !- Field 3",
        "    Until: 7:00,0.20,        !- Field 5",
        "    Until: 17:00,1.00,       !- Field 7",
        "    Until: 18:00,0.50,       !- Field 9",
        "    Until: 24:00,0.05,       !- Field 11",
        "    For: AllOtherDays,       !- Field 13",
        "    Until: 24:00,0.05;       !- Field 14",

        "  Schedule:Compact,",
        "    FanAndCoilAvailSched,    !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,1.0,        !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: WeekDays SummerDesignDay, !- Field 6",
        "    Until: 7:00,0.0,         !- Field 7",
        "    Until: 17:00,1.0,        !- Field 9",
        "    Until: 24:00,0.0,        !- Field 11",
        "    For: WinterDesignDay,    !- Field 13",
        "    Until: 24:00,0.0,        !- Field 14",
        "    For: AllOtherDays,       !- Field 16",
        "    Until: 24:00,0.0,        !- Field 17",
        "    Through: 12/31,          !- Field 19",
        "    For: AllDays,            !- Field 20",
        "    Until: 24:00,1.0;        !- Field 21",

        "  Schedule:Compact,",
        "    CoolingCoilAvailSched,   !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.0,        !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: WeekDays SummerDesignDay, !- Field 6",
        "    Until: 7:00,0.0,         !- Field 7",
        "    Until: 17:00,1.0,        !- Field 9",
        "    Until: 24:00,0.0,        !- Field 11",
        "    For: WinterDesignDay,    !- Field 13",
        "    Until: 24:00,0.0,        !- Field 14",
        "    For: AllOtherDays,       !- Field 16",
        "    Until: 24:00,0.0,        !- Field 17",
        "    Through: 12/31,          !- Field 19",
        "    For: AllDays,            !- Field 20",
        "    Until: 24:00,0.0;        !- Field 21",

        "  Schedule:Compact,",
        "    Dual Heating Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,15.0,       !- Field 3",
        "    Until: 17:00,22.0,       !- Field 5",
        "    Until: 24:00,15.0;       !- Field 7",

        "  Schedule:Compact,",
        "    Dual Cooling Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,35.0,       !- Field 3",
        "    Until: 17:00,24.0,       !- Field 5",
        "    Until: 24:00,40.0;       !- Field 7",

        "  Schedule:Compact,",
        "    Dual Zone Control Type Sched,  !- Name",
        "    Control Type,            !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,4;          !- Field 3",

        "  Schedule:Compact,",
        "    VentingSched,            !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,1.0,        !- Field 3",
        "    Until: 17:00,0.0,        !- Field 5",
        "    Until: 24:00,1.0;        !- Field 7",

        "  Schedule:Compact,",
        "    Minimum OA Sch,          !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  7:00,0.10,       !- Field 3",
        "    Until: 17:00,0.25,       !- Field 5",
        "    Until: 24:00,0.10,       !- Field 7",
        "    Through: 9/30,           !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until: 24:00,0.25,       !- Field 11",
        "    Through: 12/31,          !- Field 13",
        "    For: AllDays,            !- Field 14",
        "    Until:  7:00,0.10,       !- Field 15",
        "    Until: 17:00,0.25,       !- Field 17",
        "    Until: 24:00,0.10;       !- Field 19",

        "  People,",
        "    West Zone People,        !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    OFFICE OCCUPANCY,        !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    3.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "  People,",
        "    EAST ZONE People,        !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    OFFICE OCCUPANCY,        !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    3.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "  People,",
        "    NORTH ZONE People,       !- Name",
        "    NORTH ZONE,              !- Zone or ZoneList Name",
        "    OFFICE OCCUPANCY,        !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    4.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "  Lights,",
        "    West Zone Lights 1,      !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    1.0,                     !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "  Lights,",
        "    EAST ZONE Lights 1,      !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    1.0,                     !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "  Lights,",
        "    NORTH ZONE Lights 1,     !- Name",
        "    NORTH ZONE,              !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    1.0,                     !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "  ElectricEquipment,",
        "    West Zone ElecEq 1,      !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    2928.751,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "  ElectricEquipment,",
        "    EAST ZONE ElecEq 1,      !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    1464.375,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "  ElectricEquipment,",
        "    NORTH ZONE ElecEq 1,     !- Name",
        "    NORTH ZONE,              !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    2928.751,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "  Daylighting:Controls,",
        "    West Zone_DaylCtrl,      !- Name",
        "    West Zone,               !- Zone Name",
        "    SplitFlux,               !- Daylighting Method",
        "    ,                        !- Availability Schedule Name",
        "    Continuous,              !- Lighting Control Type",
        "    0.3,                     !- Minimum Input Power Fraction for Continuous or ContinuousOff Dimming Control",
        "    0.2,                     !- Minimum Light Output Fraction for Continuous or ContinuousOff Dimming Control",
        "    ,                        !- Number of Stepped Control Steps",
        "    1.0,                     !- Probability Lighting will be Reset When Needed in Manual Stepped Control",
        "    West Zone_DaylRefPt1,    !- Glare Calculation Daylighting Reference Point Name",
        "    180.0,                   !- Glare Calculation Azimuth Angle of View Direction Clockwise from Zone y-Axis {deg}",
        "    20.0,                    !- Maximum Allowable Discomfort Glare Index",
        "    ,                        !- DElight Gridding Resolution {m2}",
        "    West Zone_DaylRefPt1,    !- Daylighting Reference Point 1 Name",
        "    1.0,                     !- Fraction of Zone Controlled by Reference Point 1",
        "    500.;                    !- Illuminance Setpoint at Reference Point 1 {lux}",

        "  Daylighting:ReferencePoint,",
        "    West Zone_DaylRefPt1,    !- Name",
        "    West Zone,               !- Zone Name",
        "    3.048,                   !- X-Coordinate of Reference Point {m}",
        "    3.048,                   !- Y-Coordinate of Reference Point {m}",
        "    0.9;                     !- Z-Coordinate of Reference Point {m}",

        "  Curve:Biquadratic,",
        "    WindACCoolCapFT,         !- Name",
        "    0.942587793,             !- Coefficient1 Constant",
        "    0.009543347,             !- Coefficient2 x",
        "    0.000683770,             !- Coefficient3 x**2",
        "    -0.011042676,            !- Coefficient4 y",
        "    0.000005249,             !- Coefficient5 y**2",
        "    -0.000009720,            !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111,                !- Maximum Value of y",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Temperature,             !- Input Unit Type for Y",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Biquadratic,",
        "    WindACEIRFT,             !- Name",
        "    0.342414409,             !- Coefficient1 Constant",
        "    0.034885008,             !- Coefficient2 x",
        "    -0.000623700,            !- Coefficient3 x**2",
        "    0.004977216,             !- Coefficient4 y",
        "    0.000437951,             !- Coefficient5 y**2",
        "    -0.000728028,            !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111,                !- Maximum Value of y",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Temperature,             !- Input Unit Type for Y",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Quadratic,",
        "    WindACCoolCapFFF,        !- Name",
        "    0.8,                     !- Coefficient1 Constant",
        "    0.2,                     !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "  Curve:Quadratic,",
        "    WindACEIRFFF,            !- Name",
        "    1.1552,                  !- Coefficient1 Constant",
        "    -0.1808,                 !- Coefficient2 x",
        "    0.0256,                  !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "  Curve:Quadratic,",
        "    WindACPLFFPLR,           !- Name",
        "    0.85,                    !- Coefficient1 Constant",
        "    0.15,                    !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.0,                     !- Minimum Value of x",
        "    1.0;                     !- Maximum Value of x",

        "  NodeList,",
        "    OutsideAirInletNodes,    !- Name",
        "    Outside Air Inlet Node;  !- Node 1 Name",

        "  NodeList,",
        "    Zone1Inlets,             !- Name",
        "    Zone 1 NoReheat Air Outlet Node;  !- Node 1 Name",

        "  NodeList,",
        "    Zone2Inlets,             !- Name",
        "    Zone 2 Reheat Air Outlet Node;  !- Node 1 Name",

        "  NodeList,",
        "    Supply Air Temp Nodes,   !- Name",
        "    Heating Coil Air Inlet Node,  !- Node 1 Name",
        "    Air Loop Outlet Node;    !- Node 2 Name",

        "  BranchList,",
        "    Air Loop Branches,       !- Name",
        "    Air Loop Main Branch;    !- Branch 1 Name",

        "  Branch,",
        "    Air Loop Main Branch,    !- Name",
        "    ,                        !- Pressure Drop Curve Name",
        "    AirLoopHVAC:OutdoorAirSystem,  !- Component 1 Object Type",
        "    OA Sys 1,                !- Component 1 Name",
        "    Air Loop Inlet Node,     !- Component 1 Inlet Node Name",
        "    Mixed Air Node,          !- Component 1 Outlet Node Name",
        "    Fan:ConstantVolume,      !- Component 2 Object Type",
        "    Supply Fan 1,            !- Component 2 Name",
        "    Mixed Air Node,          !- Component 2 Inlet Node Name",
        "    Cooling Coil Air Inlet Node,  !- Component 2 Outlet Node Name",
        "    CoilSystem:Cooling:DX,   !- Component 3 Object Type",
        "    DX Cooling Coil System 1,!- Component 3 Name",
        "    Cooling Coil Air Inlet Node,  !- Component 3 Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- Component 3 Outlet Node Name",
        "    Coil:Heating:Fuel,       !- Component 4 Object Type",
        "    Main Heating Coil 1,     !- Component 4 Name",
        "    Heating Coil Air Inlet Node,  !- Component 4 Inlet Node Name",
        "    Air Loop Outlet Node;    !- Component 4 Outlet Node Name",

        "  AirLoopHVAC,",
        "    Typical Terminal Reheat 1,  !- Name",
        "    ,                        !- Controller List Name",
        "    Reheat System 1 Avail List,  !- Availability Manager List Name",
        "    1.16,                     !- Design Supply Air Flow Rate {m3/s}",
        "    Air Loop Branches,       !- Branch List Name",
        "    ,                        !- Connector List Name",
        "    Air Loop Inlet Node,     !- Supply Side Inlet Node Name",
        "    Return Air Mixer Outlet, !- Demand Side Outlet Node Name",
        "    Zone Equipment Inlet Node,  !- Demand Side Inlet Node Names",
        "    Air Loop Outlet Node;    !- Supply Side Outlet Node Names",

        "  AirLoopHVAC:ControllerList,",
        "    OA Sys 1 Controllers,    !- Name",
        "    Controller:OutdoorAir,   !- Controller 1 Object Type",
        "    OA Controller 1;         !- Controller 1 Name",

        "  AirLoopHVAC:OutdoorAirSystem:EquipmentList,",
        "    OA Sys 1 Equipment,      !- Name",
        "    OutdoorAir:Mixer,        !- Component 1 Object Type",
        "    OA Mixing Box 1;         !- Component 1 Name",

        "  AirLoopHVAC:OutdoorAirSystem,",
        "    OA Sys 1,                !- Name",
        "    OA Sys 1 Controllers,    !- Controller List Name",
        "    OA Sys 1 Equipment;      !- Outdoor Air Equipment List Name",

        "  OutdoorAir:NodeList,",
        "    OutsideAirInletNodes;    !- Node or NodeList Name 1",

        "  OutdoorAir:Mixer,",
        "    OA Mixing Box 1,         !- Name",
        "    Mixed Air Node,          !- Mixed Air Node Name",
        "    Outside Air Inlet Node,  !- Outdoor Air Stream Node Name",
        "    Relief Air Outlet Node,  !- Relief Air Stream Node Name",
        "    Air Loop Inlet Node;     !- Return Air Stream Node Name",

        "  AirflowNetwork:SimulationControl,",
        "    AirflowNetwork_All,      !- Name",
        "    MultizoneWithDistribution,  !- AirflowNetwork Control",
        "    INPUT,                   !- Wind Pressure Coefficient Type",
        "    ExternalNode,            !- Height Selection for Local Wind Pressure Calculation",
        "    LOWRISE,                 !- Building Type",
        "    500,                     !- Maximum Number of Iterations {dimensionless}",
        "    ZeroNodePressures,       !- Initialization Type",
        "    1.0E-04,                 !- Relative Airflow Convergence Tolerance {dimensionless}",
        "    1.0E-04,                 !- Absolute Airflow Convergence Tolerance {kg/s}",
        "    -0.5,                    !- Convergence Acceleration Limit {dimensionless}",
        "    0.0,                     !- Azimuth Angle of Long Axis of Building {deg}",
        "    1.0;                     !- Ratio of Building Width Along Short Axis to Width Along Long Axis",

        "  AirflowNetwork:MultiZone:Zone,",
        "    West Zone,               !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    0.3,                     !- Minimum Venting Open Factor {dimensionless}",
        "    5.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    10.0,                    !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0,                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",
        "    VentingSched;            !- Venting Availability Schedule Name",

        "  AirflowNetwork:MultiZone:Zone,",
        "    EAST ZONE,               !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    NORTH ZONE,              !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0,                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",
        "    VentingSched;            !- Venting Availability Schedule Name",

        "  AirflowNetwork:MultiZone:Zone,",
        "    ATTIC ZONE,              !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    ATTIC NORTH ZONE,              !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall001,           !- Surface Name",
        "    ELA-1,                   !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall001:Win001,    !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall003,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall003:Door001,   !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Wall004,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn001:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall003,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    EFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Wall005,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn002:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Wall002:Win001,    !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    0.5;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn003:Ceil001,           !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn004:Roof001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn004:Roof002,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn005:Roof001,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    Horizontal,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn005:Wall004,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    ,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Zn005:Wall005,           !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    ,              !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Component:DetailedOpening,",
        "    WiOpen1,                 !- Name",
        "    0.0001,                  !- Air Mass Flow Coefficient When Opening is Closed {kg/s-m}",
        "    0.667,                   !- Air Mass Flow Exponent When Opening is Closed {dimensionless}",
        "    NonPivoted,              !- Type of Rectangular Large Vertical Opening (LVO)",
        "    0.0,                     !- Extra Crack Length or Height of Pivoting Axis {m}",
        "    2,                       !- Number of Sets of Opening Factor Data",
        "    0.0,                     !- Opening Factor 1 {dimensionless}",
        "    0.5,                     !- Discharge Coefficient for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Width Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Opening Factor 2 {dimensionless}",
        "    0.6,                     !- Discharge Coefficient for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Width Factor for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 2 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 2 {dimensionless}",
        "    0,                       !- Opening Factor 3 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 3 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Start Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Opening Factor 4 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 4 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 4 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 4 {dimensionless}",
        "    0;                       !- Start Height Factor for Opening Factor 4 {dimensionless}",

        "  AirflowNetwork:MultiZone:Component:SimpleOpening,",
        "    DrOpen,                  !- Name",
        "    0.0001,                  !- Air Mass Flow Coefficient When Opening is Closed {kg/s-m}",
        "    0.667,                   !- Air Mass Flow Exponent When Opening is Closed {dimensionless}",
        "    0.0001,                  !- Minimum Density Difference for Two-Way Flow {kg/m3}",
        "    0.55;                    !- Discharge Coefficient {dimensionless}",

        "  AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "    ReferenceCrackConditions,!- Name",
        "    20.0,                    !- Reference Temperature {C}",
        "    101325,                  !- Reference Barometric Pressure {Pa}",
        "    0.0;                     !- Reference Humidity Ratio {kgWater/kgDryAir}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CR-1,                    !- Name",
        "    0.001,                   !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CRcri,                   !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:EffectiveLeakageArea,",
        "    ELA-1,                   !- Name",
        "    0.007,                   !- Effective Leakage Area {m2}",
        "    1.0,                     !- Discharge Coefficient {dimensionless}",
        "    4.0,                     !- Reference Pressure Difference {Pa}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    NFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    NFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    EFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    EFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    SFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    SFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    WFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    WFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    Horizontal,              !- Name",
        "    3.028,                   !- External Node Height {m}",
        "    Horizontal_WPCValue;     !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientArray,",
        "    Every 30 Degrees,        !- Name",
        "    0,                       !- Wind Direction 1 {deg}",
        "    30,                      !- Wind Direction 2 {deg}",
        "    60,                      !- Wind Direction 3 {deg}",
        "    90,                      !- Wind Direction 4 {deg}",
        "    120,                     !- Wind Direction 5 {deg}",
        "    150,                     !- Wind Direction 6 {deg}",
        "    180,                     !- Wind Direction 7 {deg}",
        "    210,                     !- Wind Direction 8 {deg}",
        "    240,                     !- Wind Direction 9 {deg}",
        "    270,                     !- Wind Direction 10 {deg}",
        "    300,                     !- Wind Direction 11 {deg}",
        "    330;                     !- Wind Direction 12 {deg}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    NFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.60,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.48;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    EFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.56;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    SFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.37,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.42;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    WFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.04;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    Horizontal_WPCValue,     !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.00,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.00,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.00;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentInletNode,      !- Name",
        "    Zone Equipment Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentOutletNode,     !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    SupplyMainNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainSplitterNode,        !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1SupplyRegisterNode, !- Name",
        "    Zone 1 NoReheat Air Outlet Node,       !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1OutletNode,         !- Name",
        "    Zone 1 Outlet Node,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2SupplyNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ReheatInlet2Node,        !- Name",
        "    Zone 2 Reheat Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2SupplyRegisterNode, !- Name",
        "    Zone 2 Reheat Air Outlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2OutletNode,         !- Name",
        "    Zone 2 Outlet Node,      !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone1ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    Zone2ReturnNode,         !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainMixerNode,           !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainReturnNode,          !- Name",
        "    Return Air Mixer Outlet, !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainInletNode,           !- Name",
        "    Air Loop Inlet Node,     !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    OA System Node,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:OutdoorAirSystem,  !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    OA Inlet Node,           !- Name",
        "    Outside Air Inlet Node,  !- Component Name or Node Name",
        "    OAMixerOutdoorAirStreamNode,  !- Component Object Type or Node Type",
        "    1.5;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanInletNode,            !- Name",
        "    Mixed Air Node,          !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanOutletNode,           !- Name",
        "    Cooling Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingInletNode,        !- Name",
        "    Heating Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingOutletNode,       !- Name",
        "    Air Loop Outlet Node,    !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Component:Leak,",
        "    MainSupplyLeak,          !- Name",
        "    0.0025,                  !- Air Mass Flow Coefficient {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:ConstantPressureDrop,",
        "    SupplyCPDComp,           !- Name",
        "    1.0;                     !- Pressure Difference Across the Component {Pa}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ZoneSupplyELR1,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ZoneSupplyELR2,          !- Name",
        "    0.01,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    59.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeakELR1,          !- Name",
        "    0.03,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    41.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeakELR2,          !- Name",
        "    0.03,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    40.0,                    !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTruck1,              !- Name",
        "    3.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTruck2,              !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.6,                     !- Hydraulic Diameter {m}",
        "    0.2827,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone1Supply,             !- Name",
        "    5.0,                     !- Duct Length {m}",
        "    0.4,                     !- Hydraulic Diameter {m}",
        "    0.1256,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone2Supply,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.39,                    !- Hydraulic Diameter {m}",
        "    0.1195,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    2.5,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone1Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    Zone2Return,             !- Name",
        "    4.0,                     !- Duct Length {m}",
        "    0.48,                    !- Hydraulic Diameter {m}",
        "    0.1809,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    1.0,                     !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneConnectionDuct,      !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    30.00,                   !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MixerConnectionDuct,     !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopReturn,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopSupply,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    1.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0001,                  !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Fan,",
        "    Supply Fan 1,            !- Fan Name",
        "    Fan:ConstantVolume;      !- Supply Fan Object Type",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    ACDXCoil 1,              !- Coil Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Main Heating Coil 1,     !- Coil Name",
        "    Coil:Heating:Fuel,       !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.00;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:TerminalUnit,",
        "    Reheat Zone 2,           !- Terminal Unit Name",
        "    AirTerminal:SingleDuct:ConstantVolume:Reheat,  !- Terminal Unit Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    0.44;                    !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link 1,             !- Name",
        "    EquipmentInletNode,      !- Node 1 Name",
        "    EquipmentOutletNode,     !- Node 2 Name",
        "    MainTruck1,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main CDP Link,           !- Name",
        "    EquipmentOutletNode,     !- Node 1 Name",
        "    SupplyMainNode,          !- Node 2 Name",
        "    SupplyCPDComp;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link 2,             !- Name",
        "    SupplyMainNode,          !- Node 1 Name",
        "    MainSplitterNode,        !- Node 2 Name",
        "    MainTruck2,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Supply1Link,        !- Name",
        "    MainSplitterNode,        !- Node 1 Name",
        "    Zone1SupplyNode,         !- Node 2 Name",
        "    Zone1Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Supply2Link,        !- Name",
        "    Zone1SupplyNode,         !- Node 1 Name",
        "    Zone1SupplyRegisterNode,        !- Node 2 Name",
        "    Zone1Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1SupplyConnectionLink,  !- Name",
        "    Zone1SupplyRegisterNode, !- Node 1 Name",
        "    West Zone,               !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1ReturnConnectionLink,  !- Name",
        "    West Zone,               !- Node 1 Name",
        "    Zone1OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Supply1Link,        !- Name",
        "    MainSplitterNode,        !- Node 1 Name",
        "    Zone2SupplyNode,         !- Node 2 Name",
        "    Zone2Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Supply2Link,        !- Name",
        "    Zone2SupplyNode,         !- Node 1 Name",
        "    ReheatInlet2Node,        !- Node 2 Name",
        "    Zone2Supply,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2ReheatCoilLink,     !- Name",
        "    ReheatInlet2Node,        !- Node 1 Name",
        "    Zone2SupplyRegisterNode, !- Node 2 Name",
        "    Reheat Zone 2;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2SupplyConnectionLink,  !- Name",
        "    Zone2SupplyRegisterNode, !- Node 1 Name",
        "    EAST ZONE,               !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2returnConnectionLink,  !- Name",
        "    EAST ZONE,               !- Node 1 Name",
        "    Zone2OutletNode,         !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Return1Link,        !- Name",
        "    Zone1OutletNode,         !- Node 1 Name",
        "    Zone1ReturnNode,         !- Node 2 Name",
        "    Zone1Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1Return2Link,        !- Name",
        "    Zone1ReturnNode,         !- Node 1 Name",
        "    MainMixerNode,           !- Node 2 Name",
        "    Zone1Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Return1Link,        !- Name",
        "    Zone2OutletNode,         !- Node 1 Name",
        "    Zone2ReturnNode,         !- Node 2 Name",
        "    Zone2Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2Return2Link,        !- Name",
        "    Zone2ReturnNode,         !- Node 1 Name",
        "    MainMixerNode,           !- Node 2 Name",
        "    Zone2Return,             !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ReturnMixerLink,         !- Name",
        "    MainMixerNode,           !- Node 1 Name",
        "    MainReturnNode,          !- Node 2 Name",
        "    MixerConnectionDuct,     !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    AirLoopReturnLink,       !- Name",
        "    MainReturnNode,          !- Node 1 Name",
        "    MainInletNode,           !- Node 2 Name",
        "    AirLoopReturn;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemInletLink,       !- Name",
        "    MainInletNode,           !- Node 1 Name",
        "    OA System Node,          !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OAMixerOutletLink,       !- Name",
        "    OA System Node,          !- Node 1 Name",
        "    FanInletNode,            !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SupplyFanLink,           !- Name",
        "    FanInletNode,            !- Node 1 Name",
        "    FanOutletNode,           !- Node 2 Name",
        "    Supply Fan 1;            !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    CoolingCoilLink,         !- Name",
        "    FanOutletNode,           !- Node 1 Name",
        "    HeatingInletNode,        !- Node 2 Name",
        "    ACDXCoil 1;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    HeatingCoilLink,         !- Name",
        "    HeatingInletNode,        !- Node 1 Name",
        "    HeatingOutletNode,       !- Node 2 Name",
        "    Main Heating Coil 1;     !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    EquipmentAirLoopLink,    !- Name",
        "    HeatingOutletNode,       !- Node 1 Name",
        "    EquipmentInletNode,      !- Node 2 Name",
        "    AirLoopSupply;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1ReturnLeakLink,     !- Name",
        "    Zone1ReturnNode,         !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    ReturnLeakELR1;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    MainSupplyLeakLink,      !- Name",
        "    SupplyMainNode,          !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    MainSupplyLeak;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone1SupplyLeakLink,     !- Name",
        "    Zone1SupplyNode,         !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    ZoneSupplyELR1;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2ReturnLeakLink,     !- Name",
        "    Zone2ReturnNode,         !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    ReturnLeakELR2;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Zone2SupplyLeakLink,     !- Name",
        "    Zone2SupplyNode,         !- Node 1 Name",
        "    ATTIC ZONE,              !- Node 2 Name",
        "    ZoneSupplyELR2;          !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemFanLink,       !- Name",
        "    OA Inlet Node,           !- Node 1 Name",
        "    OA System Node,          !- Node 2 Name",
        "    OA Fan;                  !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    OASystemReliefLink,      !- Name",
        "    OA System Node,          !- Node 1 Name",
        "    OA Inlet Node,           !- Node 2 Name",
        "    Relief Fan;              !- Component Name",

        "  AirflowNetwork:Distribution:Component:OutdoorAirFlow,",
        "    OA Fan,                  !- Name",
        "    OA Mixing Box 1,         !- Outdoor Air Mixer Name",
        "    0.001,                   !- Air Mass Flow Coefficient When No Outdoor Air Flow at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When No Outdoor Air Flow {dimensionless}",

        "  AirflowNetwork:Distribution:Component:ReliefAirFlow,",
        "    Relief Fan,              !- Name",
        "    OA Mixing Box 1,         !- Outdoor Air Mixer Name",
        "    0.001,                   !- Air Mass Flow Coefficient When No Outdoor Air Flow at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent When No Outdoor Air Flow {dimensionless}",

        "  Schedule:Compact,",
        "    Pressure Setpoint Schedule,  !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,5.0,        !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: AllDays,            !- Field 6",
        "    Until: 24:00,9.5,        !- Field 7",
        "    Through: 12/31,          !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until: 24:00,5.0;        !- Field 11",

        "  AvailabilityManagerAssignmentList,",
        "    Reheat System 1 Avail List,  !- Name",
        "    AvailabilityManager:Scheduled,  !- Availability Manager 1 Object Type",
        "    Reheat System 1 Avail;   !- Availability Manager 1 Name",

        "  AvailabilityManager:Scheduled,",
        "    Reheat System 1 Avail,   !- Name",
        "    FanAndCoilAvailSched;    !- Schedule Name",

        "  SetpointManager:SingleZone:Reheat,",
        "    Supply Air Temp Manager, !- Name",
        "    Temperature,             !- Control Variable",
        "    13.,                     !- Minimum Supply Air Temperature {C}",
        "    45.,                     !- Maximum Supply Air Temperature {C}",
        "    WEST ZONE,              !- Control Zone Name",
        "    Zone 1 Node,             !- Zone Node Name",
        "    Zone 1 NoReheat Air Outlet Node,       !- Zone Inlet Node Name",
        "    Supply Air Temp Nodes;   !- Setpoint Node or NodeList Name",

        "  Controller:OutdoorAir,",
        "    OA Controller 1,         !- Name",
        "    Relief Air Outlet Node,  !- Relief Air Outlet Node Name",
        "    Air Loop Inlet Node,     !- Return Air Node Name",
        "    Mixed Air Node,          !- Mixed Air Node Name",
        "    Outside Air Inlet Node,  !- Actuator Node Name",
        "    0.2333,                  !- Minimum Outdoor Air Flow Rate {m3/s}",
        "    1.16,                     !- Maximum Outdoor Air Flow Rate {m3/s}",
        "    NoEconomizer,            !- Economizer Control Type",
        "    ModulateFlow,            !- Economizer Control Action Type",
        "    19.,                     !- Economizer Maximum Limit Dry-Bulb Temperature {C}",
        "    ,                        !- Economizer Maximum Limit Enthalpy {J/kg}",
        "    ,                        !- Economizer Maximum Limit Dewpoint Temperature {C}",
        "    ,                        !- Electronic Enthalpy Limit Curve Name",
        "    4.,                      !- Economizer Minimum Limit Dry-Bulb Temperature {C}",
        "    NoLockout,               !- Lockout Type",
        "    FixedMinimum;            !- Minimum Limit Type",

        "  ZoneHVAC:EquipmentConnections,",
        "    West Zone,               !- Zone Name",
        "    Zone1Equipment,          !- Zone Conditioning Equipment List Name",
        "    Zone1Inlets,             !- Zone Air Inlet Node or NodeList Name",
        "    ,                        !- Zone Air Exhaust Node or NodeList Name",
        "    Zone 1 Node,             !- Zone Air Node Name",
        "    Zone 1 Outlet Node;      !- Zone Return Air Node or NodeList Name",

        "  ZoneHVAC:EquipmentConnections,",
        "    EAST ZONE,               !- Zone Name",
        "    Zone2Equipment,          !- Zone Conditioning Equipment List Name",
        "    Zone2Inlets,             !- Zone Air Inlet Node or NodeList Name",
        "    ,                        !- Zone Air Exhaust Node or NodeList Name",
        "    Zone 2 Node,             !- Zone Air Node Name",
        "    Zone 2 Outlet Node;      !- Zone Return Air Node or NodeList Name",

        "  ZoneHVAC:EquipmentList,",
        "    Zone1Equipment,          !- Name",
        "    SequentialLoad,          !- Load Distribution Scheme",
        "    ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "    Zone1NoReheat,           !- Zone Equipment 1 Name",
        "    1,                       !- Zone Equipment 1 Cooling Sequence",
        "    1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "    ,                        !- Zone Equipment 1 Sequential Cooling Fraction",
        "    ;                        !- Zone Equipment 1 Sequential Heating Fraction",

        "  ZoneHVAC:EquipmentList,",
        "    Zone2Equipment,          !- Name",
        "    SequentialLoad,          !- Load Distribution Scheme",
        "    ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "    Zone2TermReheat,         !- Zone Equipment 1 Name",
        "    1,                       !- Zone Equipment 1 Cooling Sequence",
        "    1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "    ,                        !- Zone Equipment 1 Sequential Cooling Fraction",
        "    ;                        !- Zone Equipment 1 Sequential Heating Fraction",

        "  ZoneHVAC:AirDistributionUnit,",
        "    Zone1NoReheat,         !- Name",
        "    Zone 1 NoReheat Air Outlet Node,  !- Air Distribution Unit Outlet Node Name",
        "    AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "    No Reheat Zone 1;           !- Air Terminal Name",

        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "    No Reheat Zone 1,           !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    Zone 1 NoReheat Air Inlet Node,  !- Air Inlet Node Name",
        "    Zone 1 NoReheat Air Outlet Node,  !- Air Outlet Node Name",
        "    0.64;                    !- Maximum Air Flow Rate {m3/s}",

        "  ZoneHVAC:AirDistributionUnit,",
        "    Zone2TermReheat,         !- Name",
        "    Zone 2 Reheat Air Outlet Node,  !- Air Distribution Unit Outlet Node Name",
        "    AirTerminal:SingleDuct:ConstantVolume:Reheat,  !- Air Terminal Object Type",
        "    Reheat Zone 2;           !- Air Terminal Name",

        "  CoilSystem:Cooling:DX,",
        "    DX Cooling Coil System 1,!- Name",
        "    CoolingCoilAvailSched,   !- Availability Schedule Name",
        "    Cooling Coil Air Inlet Node,  !- DX Cooling Coil System Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- DX Cooling Coil System Outlet Node Name",
        "    Heating Coil Air Inlet Node,  !- DX Cooling Coil System Sensor Node Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "    ACDXCoil 1;              !- Cooling Coil Name",

        "  AirTerminal:SingleDuct:ConstantVolume:Reheat,",
        "    Reheat Zone 2,           !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    Zone 2 Reheat Air Outlet Node,  !- Air Outlet Node Name",
        "    Zone 2 Reheat Air Inlet Node,  !- Air Inlet Node Name",
        "    0.52,                    !- Maximum Air Flow Rate {m3/s}",
        "    Coil:Heating:Fuel,       !- Reheat Coil Object Type",
        "    Reheat Coil Zone 2,      !- Reheat Coil Name",
        "    0.0,                     !- Maximum Hot Water or Steam Flow Rate {m3/s}",
        "    0.0,                     !- Minimum Hot Water or Steam Flow Rate {m3/s}",
        "    0.001;                   !- Convergence Tolerance",

        "  ZoneControl:Thermostat,",
        "    Zone 1 Thermostat,       !- Name",
        "    West Zone,               !- Zone or ZoneList Name",
        "    Dual Zone Control Type Sched,  !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    Setpoints;               !- Control 1 Name",

        "  ZoneControl:Thermostat,",
        "    Zone 2 Thermostat,       !- Name",
        "    EAST ZONE,               !- Zone or ZoneList Name",
        "    Dual Zone Control Type Sched,  !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    Setpoints;               !- Control 1 Name",

        "  AirLoopHVAC:SupplyPath,",
        "    TermReheatSupplyPath,    !- Name",
        "    Zone Equipment Inlet Node,  !- Supply Air Path Inlet Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component 1 Object Type",
        "    Zone Supply Air Splitter;!- Component 1 Name",

        "  AirLoopHVAC:ReturnPath,",
        "    TermReheatReturnPath,    !- Name",
        "    Return Air Mixer Outlet, !- Return Air Path Outlet Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component 1 Object Type",
        "    Zone Return Air Mixer;   !- Component 1 Name",

        "  AirLoopHVAC:ZoneSplitter,",
        "    Zone Supply Air Splitter,!- Name",
        "    Zone Equipment Inlet Node,  !- Inlet Node Name",
        "    Zone 1 NoReheat Air Inlet Node,  !- Outlet 1 Node Name",
        "    Zone 2 Reheat Air Inlet Node;  !- Outlet 2 Node Name",

        "  AirLoopHVAC:ZoneMixer,",
        "    Zone Return Air Mixer,   !- Name",
        "    Return Air Mixer Outlet, !- Outlet Node Name",
        "    Zone 1 Outlet Node,      !- Inlet 1 Node Name",
        "    Zone 2 Outlet Node;      !- Inlet 2 Node Name",

        "  Coil:Heating:Fuel,",
        "    Main Heating Coil 1,     !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    NaturalGas,              !- Fuel Type",
        "    0.8,                     !- Burner Efficiency",
        "    45000,                   !- Nominal Capacity {W}",
        "    Heating Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Air Loop Outlet Node,    !- Air Outlet Node Name",
        "    Air Loop Outlet Node;    !- Temperature Setpoint Node Name",

        "  Coil:Heating:Fuel,",
        "    Reheat Coil Zone 2,      !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    NaturalGas,              !- Fuel Type",
        "    1.0,                     !- Burner Efficiency",
        "    3000,                    !- Nominal Capacity {W}",
        "    Zone 2 Reheat Air Inlet Node,  !- Air Inlet Node Name",
        "    Zone 2 Reheat Air Outlet Node;  !- Air Outlet Node Name",

        "  Coil:Cooling:DX:SingleSpeed,",
        "    ACDXCoil 1,              !- Name",
        "    CoolingCoilAvailSched,   !- Availability Schedule Name",
        "    20000,                   !- Gross Rated Total Cooling Capacity {W}",
        "    0.75,                     !- Gross Rated Sensible Heat Ratio",
        "    3.0,                     !- Gross Rated Cooling COP {W/W}",
        "    1.16,                     !- Rated Air Flow Rate {m3/s}",
        "    ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    Cooling Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- Air Outlet Node Name",
        "    WindACCoolCapFT,         !- Total Cooling Capacity Function of Temperature Curve Name",
        "    WindACCoolCapFFF,        !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "    WindACEIRFT,             !- Energy Input Ratio Function of Temperature Curve Name",
        "    WindACEIRFFF,            !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "    WindACPLFFPLR;           !- Part Load Fraction Correlation Curve Name",

        "  Fan:ConstantVolume,",
        "    Supply Fan 1,            !- Name",
        "    FanAndCoilAvailSched,    !- Availability Schedule Name",
        "    0.7,                     !- Fan Total Efficiency",
        "    600.0,                   !- Pressure Rise {Pa}",
        "    1.16,                     !- Maximum Flow Rate {m3/s}",
        "    0.9,                     !- Motor Efficiency",
        "    1.0,                     !- Motor In Airstream Fraction",
        "    Mixed Air Node,          !- Air Inlet Node Name",
        "    Cooling Coil Air Inlet Node;  !- Air Outlet Node Name",

        "  ThermostatSetpoint:DualSetpoint,",
        "    Setpoints,               !- Name",
        "    Dual Heating Setpoints,  !- Heating Setpoint Temperature Schedule Name",
        "    Dual Cooling Setpoints;  !- Cooling Setpoint Temperature Schedule Name",

        "ScheduleTypeLimits,",
        "  HVACTemplate Any Number;                                 !- Name",

        "Schedule:Compact,",
        "  HVACTemplate-Always 1,                                   !- Name",
        "  HVACTemplate Any Number,                                 !- Schedule Type Limits Name",
        "  Through: 12/31,                                          !- Field 1",
        "  For: AllDays,                                            !- Field 2",
        "  Until: 24:00,                                            !- Field 3",
        "  1;                                                       !- Field 4",

        "Site:GroundTemperature:BuildingSurface,",
        "  20.03,                                                   !- January Ground Temperature",
        "  20.03,                                                   !- February Ground Temperature",
        "  20.13,                                                   !- March Ground Temperature",
        "  20.30,                                                   !- April Ground Temperature",
        "  20.43,                                                   !- May Ground Temperature",
        "  20.52,                                                   !- June Ground Temperature",
        "  20.62,                                                   !- July Ground Temperature",
        "  20.77,                                                   !- August Ground Temperature",
        "  20.78,                                                   !- September Ground Temperature",
        "  20.55,                                                   !- October Ground Temperature",
        "  20.44,                                                   !- November Ground Temperature",
        "  20.20;                                                   !- December Ground Temperature",

        "Output:Diagnostics,DisplayExtraWarnings;",

        "Output:Diagnostics,DisplayUnusedSchedules;"});

    ASSERT_TRUE(process_idf(idf_objects));

    bool ErrorsFound = false;
    // Read objects
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetWindowGlassSpectralData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    Material::GetMaterialData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    SurfaceGeometry::GetGeometryParameters(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosBldgRotAppGonly = 1.0;
    state->dataSurfaceGeometry->SinBldgRotAppGonly = 0.0;
    SurfaceGeometry::GetSurfaceData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    // Read AirflowNetwork inputs
    state->afn->get_input();

    state->afn->AirflowNetworkFanActivated = true;
    state->dataEnvrn->OutDryBulbTemp = -17.29025;
    state->dataEnvrn->OutHumRat = 0.0008389;
    state->dataEnvrn->OutBaroPress = 99063.0;
    state->dataEnvrn->WindSpeed = 4.9;
    state->dataEnvrn->WindDir = 270.0;

    for (int i = 1; i <= 32; ++i) {
        state->afn->AirflowNetworkNodeSimu(i).TZ = 23.0;
        state->afn->AirflowNetworkNodeSimu(i).WZ = 0.0008400;
        if ((i > 4 && i < 10) || i == 32) {
            state->afn->AirflowNetworkNodeSimu(i).TZ = DataEnvironment::OutDryBulbTempAt(*state, state->afn->AirflowNetworkNodeData(i).NodeHeight);
            state->afn->AirflowNetworkNodeSimu(i).WZ = state->dataEnvrn->OutHumRat;
        }
    }

    // Set up node values
    state->dataLoopNodes->Node.allocate(17);

    state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(1).InletNode).MassFlowRate = 1.40;
    state->afn->DisSysCompCVFData(1).FlowRate = state->dataLoopNodes->Node(state->afn->DisSysCompCVFData(1).InletNode).MassFlowRate;

    state->afn->DisSysCompOutdoorAirData(1).InletNode = 6;
    state->dataLoopNodes->Node(state->afn->DisSysCompOutdoorAirData(1).InletNode).MassFlowRate = 0.1095108;

    if (state->afn->DisSysCompReliefAirData(1).InletNode == 0) {
        state->afn->DisSysCompReliefAirData(1).OutletNode = 1;
    }

    state->dataAirLoop->AirLoopAFNInfo.allocate(1);
    state->dataAirLoop->AirLoopAFNInfo(1).LoopFanOperationMode = 0.0;
    state->dataAirLoop->AirLoopAFNInfo(1).LoopOnOffFanPartLoadRatio = 0.0;

    state->afn->AirflowNetworkFanActivated = false;

    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(5);
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(2).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(3).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(4).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(5).MAT = 23.0;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(1).ZoneAirHumRat = 0.001;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(2).ZoneAirHumRat = 0.001;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(3).ZoneAirHumRat = 0.001;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(4).ZoneAirHumRat = 0.001;
    state->dataZoneTempPredictorCorrector->zoneHeatBalance(5).ZoneAirHumRat = 0.001;

    DataZoneEquipment::GetZoneEquipmentData(*state);
    ZoneAirLoopEquipmentManager::GetZoneAirLoopEquipment(*state);
    SimAirServingZones::GetAirPathData(*state);

    // Read AirflowNetwork inputs
    state->afn->get_input();

    state->afn->AirflowNetworkGetInputFlag = false;
    state->dataZoneEquip->ZoneEquipConfig(1).InletNodeAirLoopNum(1) = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).ReturnNodeAirLoopNum(1) = 1;
    state->dataZoneEquip->ZoneEquipConfig(2).InletNodeAirLoopNum(1) = 1;
    state->dataZoneEquip->ZoneEquipConfig(2).ReturnNodeAirLoopNum(1) = 1;
    state->afn->DisSysNodeData(9).EPlusNodeNum = 50;
    // AirflowNetwork::AirflowNetworkExchangeData.allocate(5);
    state->afn->manage_balance(true);
    EXPECT_EQ(state->afn->DisSysCompCVFData(1).AirLoopNum, 1);
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestZoneVentingAirBoundary)
{
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "SALA DE AULA";

    state->dataSurface->Surface.allocate(3);
    state->dataSurface->Surface(1).Name = "WINDOW AULA 1";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "SALA DE AULA";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = DataSurfaces::ExternalEnvironment;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).IsAirBoundarySurf = false;
    state->dataSurface->Surface(1).Tilt = 90.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(2).Name = "AIR WALL AULA 2";
    state->dataSurface->Surface(2).Zone = 1;
    state->dataSurface->Surface(2).ZoneName = "SALA DE AULA";
    state->dataSurface->Surface(2).Azimuth = 180.0;
    state->dataSurface->Surface(2).BaseSurf = 2;     // Make this a base surface
    state->dataSurface->Surface(2).ExtBoundCond = 3; // Make this is an interzone surface
    state->dataSurface->Surface(2).HeatTransSurf = false;
    state->dataSurface->Surface(2).IsAirBoundarySurf = true;
    state->dataSurface->Surface(2).Tilt = 90.0;
    state->dataSurface->Surface(2).Sides = 4;
    state->dataSurface->Surface(3).Name = "AIR WALL AULA 2b";
    state->dataSurface->Surface(3).Zone = 1;
    state->dataSurface->Surface(3).ZoneName = "SALA DE AULA";
    state->dataSurface->Surface(3).Azimuth = 0.0;
    state->dataSurface->Surface(3).BaseSurf = 3;     // Make this a base surface
    state->dataSurface->Surface(3).ExtBoundCond = 2; // Make this is an interzone surface
    state->dataSurface->Surface(3).HeatTransSurf = false;
    state->dataSurface->Surface(3).IsAirBoundarySurf = true;
    state->dataSurface->Surface(3).Tilt = 90.0;
    state->dataSurface->Surface(3).Sides = 4;

    SurfaceGeometry::AllocateSurfaceWindows(*state, 2);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;
    state->dataSurface->SurfWinOriginalClass(2) = DataSurfaces::SurfaceClass::Window;
    state->dataGlobal->NumOfZones = 1;

    std::string const idf_objects = delimited_string({
        "Schedule:Constant,OnSch,,1.0;",
        "Schedule:Constant,Aula people sched,,0.0;",
        "Schedule:Constant,Sempre 21,,21.0;",
        "AirflowNetwork:SimulationControl,",
        "  NaturalVentilation, !- Name",
        "  MultizoneWithoutDistribution, !- AirflowNetwork Control",
        "  SurfaceAverageCalculation, !- Wind Pressure Coefficient Type",
        "  , !- Height Selection for Local Wind Pressure Calculation",
        "  LOWRISE, !- Building Type",
        "  1000, !- Maximum Number of Iterations{ dimensionless }",
        "  LinearInitializationMethod, !- Initialization Type",
        "  0.0001, !- Relative Airflow Convergence Tolerance{ dimensionless }",
        "  0.0001, !- Absolute Airflow Convergence Tolerance{ kg / s }",
        "  -0.5, !- Convergence Acceleration Limit{ dimensionless }",
        "  90, !- Azimuth Angle of Long Axis of Building{ deg }",
        "  0.36;                    !- Ratio of Building Width Along Short Axis to Width Along Long Axis",
        "AirflowNetwork:MultiZone:Zone,",
        "  sala de aula, !- Zone Name",
        "  Temperature, !- Ventilation Control Mode",
        "  Sempre 21, !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "  1, !- Minimum Venting Open Factor{ dimensionless }",
        "  , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "  100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "  , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "  300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "  Aula people sched, !- Venting Availability Schedule Name",
        "  Standard;                !- Single Sided Wind Pressure Coefficient Algorithm",
        "AirflowNetwork:MultiZone:Surface,",
        "  window aula 1, !- Surface Name",
        "  Simple Window, !- Leakage Component Name",
        "  , !- External Node Name",
        "  1, !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "  Temperature, !- Ventilation Control Mode",
        "  Sempre 21, !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "  , !- Minimum Venting Open Factor{ dimensionless }",
        "  , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "  100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "  , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "  300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "  Aula people sched;       !- Venting Availability Schedule Name",
        "AirflowNetwork:MultiZone:Surface,",
        "  Air Wall aula 2, !- Surface Name",
        "  Simple Window, !- Leakage Component Name",
        "  , !- External Node Name",
        "  1, !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "  Temperature, !- Ventilation Control Mode",
        "  Sempre 21, !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "  1, !- Minimum Venting Open Factor{ dimensionless }",
        "  , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "  100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "  , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "  300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "  Aula people sched;       !- Venting Availability Schedule Name",
        "AirflowNetwork:MultiZone:Component:SimpleOpening,",
        "  Simple Window, !- Name",
        "  0.0010, !- Air Mass Flow Coefficient When Opening is Closed{ kg / s - m }",
        "  0.65, !- Air Mass Flow Exponent When Opening is Closed{ dimensionless }",
        "  0.01, !- Minimum Density Difference for Two - Way Flow{ kg / m3 }",
        "  0.78;                    !- Discharge Coefficient{ dimensionless }",
    });

    ASSERT_TRUE(process_idf(idf_objects));
    state->afn->get_input();
    // Expect warnings about the air boundary surface
    EXPECT_TRUE(has_err_output(false));
    std::string const expectedErrString = delimited_string(
        {"   ** Warning ** AirflowNetwork::Solver::get_input: AirflowNetwork:MultiZone:Surface=\"AIR WALL AULA 2\" is an air boundary surface.",
         "   **   ~~~   ** Ventilation Control Mode = TEMPERATURE is not valid. Resetting to Constant.",
         "   ** Warning ** AirflowNetwork::Solver::get_input: AirflowNetwork:MultiZone:Surface=\"AIR WALL AULA 2\" is an air boundary surface.",
         "   **   ~~~   ** Venting Availability Schedule will be ignored, venting is always available."});
    EXPECT_TRUE(compare_err_stream(expectedErrString, true));

    // MultizoneSurfaceData(1) is connected to a normal heat transfer surface -
    // venting schedule should be non-zero and venting method should be ZoneLevel
    auto GetIndex = UtilityRoutines::FindItemInList(state->afn->MultizoneSurfaceData(1).VentingSchName,
                                                    state->dataScheduleMgr->Schedule({1, state->dataScheduleMgr->NumSchedules}));
    EXPECT_GT(GetIndex, 0);
    EXPECT_EQ(GetIndex, state->afn->MultizoneSurfaceData(1).VentingSchNum);
    EXPECT_TRUE(compare_enums(state->afn->MultizoneSurfaceData(1).VentSurfCtrNum, AirflowNetwork::VentControlType::Temp));

    // MultizoneSurfaceData(2) is connected to an air boundary surface
    // venting schedule should be zero and venting method should be Constant
    EXPECT_EQ(0, state->afn->MultizoneSurfaceData(2).VentingSchNum);
    EXPECT_TRUE(compare_enums(state->afn->MultizoneSurfaceData(2).VentSurfCtrNum, AirflowNetwork::VentControlType::Const));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestNoZoneEqpSupportZoneERV)
{
    // Create zone
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ZONE 1";

    // Create surfaces
    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    // Create air system
    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    // Create air nodes
    state->dataLoopNodes->NumOfNodes = 5;
    state->dataLoopNodes->Node.allocate(5);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(2).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(3).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(4).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(5).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(5);
    state->dataLoopNodes->NodeID(1) = "ZONE 1 AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ZONE 1 AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    // Connect zone to air node
    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ZONE 1";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;

    // One AirflowNetwork:MultiZone:Zone object
    state->afn->AirflowNetworkNumOfZones = 1;
    state->afn->MultizoneZoneData.allocate(1);
    state->afn->MultizoneZoneData(1).ZoneNum = 1;
    state->afn->MultizoneZoneData(1).ZoneName = "ZONE 1";

    // Assume only one AirflowNetwork:Distribution:Node object is set for the Zone Air Node
    state->afn->AirflowNetworkNumOfNodes = 1;
    state->afn->AirflowNetworkNodeData.allocate(1);
    state->afn->AirflowNetworkNodeData(1).Name = "ZONE 1";
    state->afn->AirflowNetworkNodeData(1).EPlusZoneNum = 1;

    state->afn->SplitterNodeNumbers.allocate(2);
    state->afn->SplitterNodeNumbers(1) = 0;
    state->afn->SplitterNodeNumbers(2) = 0;

    // Set flag to support zone equipment
    state->afn->simulation_control.allow_unsupported_zone_equipment = false;

    // Create Fans
    Real64 supplyFlowRate = 0.005;
    Real64 exhaustFlowRate = 0.005;
    state->dataFans->Fan.allocate(2);
    state->dataFans->Fan(1).InletNodeNum = 2;
    state->dataFans->Fan(1).OutletNodeNum = 3;
    state->dataFans->Fan(1).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(1).FanName = "SupplyFan";
    state->dataFans->Fan(1).MaxAirFlowRate = supplyFlowRate;
    state->dataLoopNodes->NodeID(2) = "SupplyFanInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  2,
                                                  state->dataLoopNodes->NodeID(2),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(3) = "SupplyFanOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  3,
                                                  state->dataLoopNodes->NodeID(3),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataFans->Fan(2).InletNodeNum = 4;
    state->dataFans->Fan(2).OutletNodeNum = 5;
    state->dataFans->Fan(2).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(2).FanName = "ExhaustFan";
    state->dataFans->Fan(2).MaxAirFlowRate = exhaustFlowRate;
    state->dataLoopNodes->NodeID(4) = "SupplyExhaustInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  4,
                                                  state->dataLoopNodes->NodeID(4),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(2).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(5) = "SupplyExhaustOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  5,
                                                  state->dataLoopNodes->NodeID(5),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(2).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);

    // Create ERV
    state->dataHVACStandAloneERV->StandAloneERV.allocate(1);
    state->dataHVACStandAloneERV->GetERVInputFlag = false;
    state->dataHVACStandAloneERV->NumStandAloneERVs = 1;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirVolFlow = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirVolFlow = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignSAFanVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignEAFanVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignHXVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirFanName = state->dataFans->Fan(1).FanName;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirFanIndex = 1;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirInletNode = 2;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirFanName = state->dataFans->Fan(2).FanName;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirFanIndex = 2;
    state->dataHVACStandAloneERV->StandAloneERV(1).HeatExchangerTypeNum = HX_AIRTOAIR_GENERIC;
    state->dataHVACStandAloneERV->StandAloneERV(1).HeatExchangerName = "ERV Heat Exchanger";

    // Check validation and expected errors
    ASSERT_THROW(state->afn->validate_distribution(), std::runtime_error);
    std::string const error_string = delimited_string({
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanInletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanOutletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyExhaustInletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyExhaustOutletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node object.",
        "   **  Fatal  ** AirflowNetwork::Solver::validate_distribution: Program terminates for preceding reason(s).",
        "   ...Summary of Errors that led to program termination:",
        "   ..... Reference severe error count=4",
        "   ..... Last severe error=AirflowNetwork::Solver::validate_distribution: 'SupplyExhaustOutletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node "
        "object.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestZoneEqpSupportZoneERV)
{
    // Create zone
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ZONE 1";

    // Create surfaces
    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    // Create air system
    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    // Create air nodes
    state->dataLoopNodes->NumOfNodes = 5;
    state->dataLoopNodes->Node.allocate(5);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(2).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(3).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(4).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(5).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(5);
    state->dataLoopNodes->NodeID(1) = "ZONE 1 AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ZONE 1 AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    // Connect zone to air node
    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ZONE 1";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;

    // One AirflowNetwork:MultiZone:Zone object
    state->afn->AirflowNetworkNumOfZones = 1;
    state->afn->MultizoneZoneData.allocate(1);
    state->afn->MultizoneZoneData(1).ZoneNum = 1;
    state->afn->MultizoneZoneData(1).ZoneName = "ZONE 1";

    // Assume only one AirflowNetwork:Distribution:Node object is set for the Zone Air Node
    state->afn->AirflowNetworkNumOfNodes = 1;
    state->afn->AirflowNetworkNodeData.allocate(1);
    state->afn->AirflowNetworkNodeData(1).Name = "ZONE 1";
    state->afn->AirflowNetworkNodeData(1).EPlusZoneNum = 1;

    state->afn->SplitterNodeNumbers.allocate(2);
    state->afn->SplitterNodeNumbers(1) = 0;
    state->afn->SplitterNodeNumbers(2) = 0;

    // Set flag to support zone equipment
    state->afn->simulation_control.allow_unsupported_zone_equipment = true;

    // Create Fans
    Real64 supplyFlowRate = 0.005;
    Real64 exhaustFlowRate = 0.005;
    state->dataFans->Fan.allocate(2);
    state->dataFans->Fan(1).InletNodeNum = 2;
    state->dataFans->Fan(1).OutletNodeNum = 3;
    state->dataFans->Fan(1).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(1).FanName = "SupplyFan";
    state->dataFans->Fan(1).MaxAirFlowRate = supplyFlowRate;
    state->dataLoopNodes->NodeID(2) = "SupplyFanInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  2,
                                                  state->dataLoopNodes->NodeID(2),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(3) = "SupplyFanOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  3,
                                                  state->dataLoopNodes->NodeID(3),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataFans->Fan(2).InletNodeNum = 4;
    state->dataFans->Fan(2).OutletNodeNum = 5;
    state->dataFans->Fan(2).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(2).FanName = "ExhaustFan";
    state->dataFans->Fan(2).MaxAirFlowRate = exhaustFlowRate;
    state->dataLoopNodes->NodeID(4) = "SupplyExhaustInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  4,
                                                  state->dataLoopNodes->NodeID(4),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(2).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(5) = "SupplyExhaustOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  5,
                                                  state->dataLoopNodes->NodeID(5),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(2).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);

    // Create ERV
    state->dataHVACStandAloneERV->StandAloneERV.allocate(1);
    state->dataHVACStandAloneERV->GetERVInputFlag = false;
    state->dataHVACStandAloneERV->NumStandAloneERVs = 1;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirVolFlow = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirVolFlow = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignSAFanVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignEAFanVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignHXVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirFanName = state->dataFans->Fan(1).FanName;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirFanIndex = 1;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirInletNode = 2;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirFanName = state->dataFans->Fan(2).FanName;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirFanIndex = 2;
    state->dataHVACStandAloneERV->StandAloneERV(1).HeatExchangerTypeNum = HX_AIRTOAIR_GENERIC;
    state->dataHVACStandAloneERV->StandAloneERV(1).HeatExchangerName = "ERV Heat Exchanger";

    // Check validation and expected warning
    state->afn->validate_distribution();

    EXPECT_TRUE(compare_err_stream(
        "   ** Warning ** AirflowNetwork::Solver::validate_distribution: A ZoneHVAC:EnergyRecoveryVentilator is simulated along with an "
        "AirflowNetwork but is not included in the AirflowNetwork.\n",
        true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestZoneEqpSupportUnbalancedZoneERV)
{
    // Create zone
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ZONE 1";

    // Create surfaces
    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    // Create air system
    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    // Create air nodes
    state->dataLoopNodes->NumOfNodes = 5;
    state->dataLoopNodes->Node.allocate(5);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(2).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(3).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(4).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(5).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(5);
    state->dataLoopNodes->NodeID(1) = "ZONE 1 AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ZONE 1 AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    // Connect zone to air node
    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ZONE 1";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;

    // One AirflowNetwork:MultiZone:Zone object
    state->afn->AirflowNetworkNumOfZones = 1;
    state->afn->MultizoneZoneData.allocate(1);
    state->afn->MultizoneZoneData(1).ZoneNum = 1;
    state->afn->MultizoneZoneData(1).ZoneName = "ZONE 1";

    // Assume only one AirflowNetwork:Distribution:Node object is set for the Zone Air Node
    state->afn->AirflowNetworkNumOfNodes = 1;
    state->afn->AirflowNetworkNodeData.allocate(1);
    state->afn->AirflowNetworkNodeData(1).Name = "ZONE 1";
    state->afn->AirflowNetworkNodeData(1).EPlusZoneNum = 1;

    state->afn->SplitterNodeNumbers.allocate(2);
    state->afn->SplitterNodeNumbers(1) = 0;
    state->afn->SplitterNodeNumbers(2) = 0;

    // Set flag to support zone equipment
    state->afn->simulation_control.allow_unsupported_zone_equipment = true;

    // Create Fans
    Real64 supplyFlowRate = 0.005;
    Real64 exhaustFlowRate = 0.003;
    state->dataFans->Fan.allocate(2);
    state->dataFans->Fan(1).InletNodeNum = 2;
    state->dataFans->Fan(1).OutletNodeNum = 3;
    state->dataFans->Fan(1).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(1).FanName = "SupplyFan";
    state->dataFans->Fan(1).MaxAirFlowRate = supplyFlowRate;
    state->dataLoopNodes->NodeID(2) = "SupplyFanInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  2,
                                                  state->dataLoopNodes->NodeID(2),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(3) = "SupplyFanOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  3,
                                                  state->dataLoopNodes->NodeID(3),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataFans->Fan(2).InletNodeNum = 4;
    state->dataFans->Fan(2).OutletNodeNum = 5;
    state->dataFans->Fan(2).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(2).FanName = "ExhaustFan";
    state->dataFans->Fan(2).MaxAirFlowRate = exhaustFlowRate;
    state->dataLoopNodes->NodeID(4) = "SupplyExhaustInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  4,
                                                  state->dataLoopNodes->NodeID(4),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(2).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(5) = "SupplyExhaustOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  5,
                                                  state->dataLoopNodes->NodeID(5),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(2).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);

    // Create ERV
    state->dataHVACStandAloneERV->StandAloneERV.allocate(1);
    state->dataHVACStandAloneERV->GetERVInputFlag = false;
    state->dataHVACStandAloneERV->NumStandAloneERVs = 1;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirVolFlow = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirVolFlow = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignSAFanVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignEAFanVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).DesignHXVolFlowRate = 0.005;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirFanName = state->dataFans->Fan(1).FanName;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirFanIndex = 1;
    state->dataHVACStandAloneERV->StandAloneERV(1).SupplyAirInletNode = 2;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirFanName = state->dataFans->Fan(2).FanName;
    state->dataHVACStandAloneERV->StandAloneERV(1).ExhaustAirFanIndex = 2;
    state->dataHVACStandAloneERV->StandAloneERV(1).HeatExchangerTypeNum = HX_AIRTOAIR_GENERIC;
    state->dataHVACStandAloneERV->StandAloneERV(1).HeatExchangerName = "ERV Heat Exchanger";

    // Check validation and expected errors
    ASSERT_THROW(state->afn->validate_distribution(), std::runtime_error);
    std::string const error_string = delimited_string({
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanInletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanOutletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyExhaustInletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyExhaustOutletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node object.",
        "   **  Fatal  ** AirflowNetwork::Solver::validate_distribution: Program terminates for preceding reason(s).",
        "   ...Summary of Errors that led to program termination:",
        "   ..... Reference severe error count=4",
        "   ..... Last severe error=AirflowNetwork::Solver::validate_distribution: 'SupplyExhaustOutletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node "
        "object.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestNoZoneEqpSupportHPWH)
{
    // Create zone
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ZONE 1";

    // Create surfaces
    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    // Create air system
    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    // Create air nodes
    state->dataLoopNodes->NumOfNodes = 3;
    state->dataLoopNodes->Node.allocate(3);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(2).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(3).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(3);
    state->dataLoopNodes->NodeID(1) = "ZONE 1 AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ZONE 1 AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    // Connect zone to air node
    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ZONE 1";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;

    // One AirflowNetwork:MultiZone:Zone object
    state->afn->AirflowNetworkNumOfZones = 1;
    state->afn->MultizoneZoneData.allocate(1);
    state->afn->MultizoneZoneData(1).ZoneNum = 1;
    state->afn->MultizoneZoneData(1).ZoneName = "ZONE 1";

    // Assume only one AirflowNetwork:Distribution:Node object is set for the Zone Air Node
    state->afn->AirflowNetworkNumOfNodes = 1;
    state->afn->AirflowNetworkNodeData.allocate(1);
    state->afn->AirflowNetworkNodeData(1).Name = "ZONE 1";
    state->afn->AirflowNetworkNodeData(1).EPlusZoneNum = 1;

    state->afn->SplitterNodeNumbers.allocate(2);
    state->afn->SplitterNodeNumbers(1) = 0;
    state->afn->SplitterNodeNumbers(2) = 0;

    // Set flag to support zone equipment
    state->afn->simulation_control.allow_unsupported_zone_equipment = false;

    // Create Fan
    state->dataFans->Fan.allocate(1);
    state->dataFans->Fan(1).InletNodeNum = 2;
    state->dataFans->Fan(1).OutletNodeNum = 3;
    state->dataFans->Fan(1).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(1).FanName = "SupplyFan";
    state->dataLoopNodes->NodeID(2) = "SupplyFanInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  2,
                                                  state->dataLoopNodes->NodeID(2),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(3) = "SupplyFanOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  3,
                                                  state->dataLoopNodes->NodeID(3),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);

    // Create HPWH
    state->dataWaterThermalTanks->HPWaterHeater.allocate(1);
    state->dataWaterThermalTanks->getWaterThermalTankInputFlag = false;
    state->dataWaterThermalTanks->numHeatPumpWaterHeater = 1;
    state->dataWaterThermalTanks->HPWaterHeater(1).InletAirConfiguration = WaterThermalTanks::WTTAmbientTemp::TempZone;
    state->dataWaterThermalTanks->HPWaterHeater(1).FanName = state->dataFans->Fan(1).FanName;
    state->dataWaterThermalTanks->HPWaterHeater(1).FanType = "Fan:OnOff";
    state->dataWaterThermalTanks->HPWaterHeater(1).FanOutletNode = 3;

    // Check validation and expected errors
    ASSERT_THROW(state->afn->validate_distribution(), std::runtime_error);
    std::string const error_string = delimited_string({
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanInletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanOutletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   **  Fatal  ** AirflowNetwork::Solver::validate_distribution: Program terminates for preceding reason(s).",
        "   ...Summary of Errors that led to program termination:",
        "   ..... Reference severe error count=2",
        "   ..... Last severe error=AirflowNetwork::Solver::validate_distribution: 'SupplyFanOutletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node object.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestZoneEqpSupportHPWH)
{
    // Create zone
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ZONE 1";

    // Create surfaces
    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    // Create air system
    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    // Create air nodes
    state->dataLoopNodes->NumOfNodes = 3;
    state->dataLoopNodes->Node.allocate(3);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(2).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(3).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(3);
    state->dataLoopNodes->NodeID(1) = "ZONE 1 AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ZONE 1 AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    // Connect zone to air node
    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ZONE 1";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;

    // One AirflowNetwork:MultiZone:Zone object
    state->afn->AirflowNetworkNumOfZones = 1;
    state->afn->MultizoneZoneData.allocate(1);
    state->afn->MultizoneZoneData(1).ZoneNum = 1;
    state->afn->MultizoneZoneData(1).ZoneName = "ZONE 1";

    // Assume only one AirflowNetwork:Distribution:Node object is set for the Zone Air Node
    state->afn->AirflowNetworkNumOfNodes = 1;
    state->afn->AirflowNetworkNodeData.allocate(1);
    state->afn->AirflowNetworkNodeData(1).Name = "ZONE 1";
    state->afn->AirflowNetworkNodeData(1).EPlusZoneNum = 1;

    state->afn->SplitterNodeNumbers.allocate(2);
    state->afn->SplitterNodeNumbers(1) = 0;
    state->afn->SplitterNodeNumbers(2) = 0;

    // Set flag to support zone equipment
    state->afn->simulation_control.allow_unsupported_zone_equipment = true;

    // Create Fan
    state->dataFans->Fan.allocate(1);
    state->dataFans->Fan(1).InletNodeNum = 2;
    state->dataFans->Fan(1).OutletNodeNum = 3;
    state->dataFans->Fan(1).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(1).FanName = "SupplyFan";
    state->dataLoopNodes->NodeID(2) = "SupplyFanInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  2,
                                                  state->dataLoopNodes->NodeID(2),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(3) = "SupplyFanOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  3,
                                                  state->dataLoopNodes->NodeID(3),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);

    // Create HPWH
    state->dataWaterThermalTanks->HPWaterHeater.allocate(1);
    state->dataWaterThermalTanks->getWaterThermalTankInputFlag = false;
    state->dataWaterThermalTanks->numHeatPumpWaterHeater = 1;
    state->dataWaterThermalTanks->HPWaterHeater(1).InletAirConfiguration = WaterThermalTanks::WTTAmbientTemp::TempZone;
    state->dataWaterThermalTanks->HPWaterHeater(1).FanName = state->dataFans->Fan(1).FanName;
    state->dataWaterThermalTanks->HPWaterHeater(1).FanType = "Fan:OnOff";
    state->dataWaterThermalTanks->HPWaterHeater(1).FanOutletNode = 3;

    // Check validation and expected warning
    state->afn->validate_distribution();
    EXPECT_TRUE(compare_err_stream(
        "   ** Warning ** AirflowNetwork::Solver::validate_distribution: Heat pump water heater is simulated along with an AirflowNetwork "
        "but is not included in the AirflowNetwork.\n",
        true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestZoneEqpSupportHPWHZoneAndOA)
{
    // Create zone
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(1);
    state->dataHeatBal->Zone(1).Name = "ZONE 1";

    // Create surfaces
    state->dataSurface->Surface.allocate(1);
    state->dataSurface->Surface(1).Name = "ZN004:ROOF001";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;
    state->dataSurface->Surface(1).Name = "ZN004:ROOF002";
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(1).ZoneName = "ZONE 1";
    state->dataSurface->Surface(1).Azimuth = 0.0;
    state->dataSurface->Surface(1).ExtBoundCond = 0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(1).Tilt = 180.0;
    state->dataSurface->Surface(1).Sides = 4;

    state->dataSurface->SurfWinOriginalClass.allocate(1);
    state->dataSurface->SurfWinOriginalClass(1) = DataSurfaces::SurfaceClass::Window;

    // Create air system
    state->dataAirSystemsData->PrimaryAirSystems.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).NumBranches = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).TotalComponents = 1;
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp.allocate(1);
    state->dataAirSystemsData->PrimaryAirSystems(1).Branch(1).Comp(1).TypeOf = "Fan:ConstantVolume";

    // Create air nodes
    state->dataLoopNodes->NumOfNodes = 3;
    state->dataLoopNodes->Node.allocate(3);
    state->dataLoopNodes->Node(1).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(2).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->Node(3).FluidType = DataLoopNode::NodeFluidType::Air;
    state->dataLoopNodes->NodeID.allocate(3);
    state->dataLoopNodes->NodeID(1) = "ZONE 1 AIR NODE";
    bool errFlag{false};
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  1,
                                                  "ZONE 1 AIR NODE",
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  "Object1",
                                                  DataLoopNode::ConnectionType::ZoneNode,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    EXPECT_FALSE(errFlag);

    // Connect zone to air node
    state->dataZoneEquip->ZoneEquipConfig.allocate(1);
    state->dataZoneEquip->ZoneEquipConfig(1).IsControlled = true;
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneName = "ZONE 1";
    state->dataZoneEquip->ZoneEquipConfig(1).ZoneNode = 1;
    state->dataZoneEquip->ZoneEquipConfig(1).NumInletNodes = 0;
    state->dataZoneEquip->ZoneEquipConfig(1).NumReturnNodes = 0;

    // One AirflowNetwork:MultiZone:Zone object
    state->afn->AirflowNetworkNumOfZones = 1;
    state->afn->MultizoneZoneData.allocate(1);
    state->afn->MultizoneZoneData(1).ZoneNum = 1;
    state->afn->MultizoneZoneData(1).ZoneName = "ZONE 1";

    // Assume only one AirflowNetwork:Distribution:Node object is set for the Zone Air Node
    state->afn->AirflowNetworkNumOfNodes = 1;
    state->afn->AirflowNetworkNodeData.allocate(1);
    state->afn->AirflowNetworkNodeData(1).Name = "ZONE 1";
    state->afn->AirflowNetworkNodeData(1).EPlusZoneNum = 1;

    state->afn->SplitterNodeNumbers.allocate(2);
    state->afn->SplitterNodeNumbers(1) = 0;
    state->afn->SplitterNodeNumbers(2) = 0;

    // Set flag to support zone equipment
    state->afn->simulation_control.allow_unsupported_zone_equipment = true;

    // Create Fan
    state->dataFans->Fan.allocate(1);
    state->dataFans->Fan(1).InletNodeNum = 2;
    state->dataFans->Fan(1).OutletNodeNum = 3;
    state->dataFans->Fan(1).FanType_Num = FanType_SimpleOnOff;
    state->dataFans->Fan(1).FanName = "SupplyFan";
    state->dataLoopNodes->NodeID(2) = "SupplyFanInletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  2,
                                                  state->dataLoopNodes->NodeID(2),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Inlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);
    state->dataLoopNodes->NodeID(3) = "SupplyFanOutletNode";
    BranchNodeConnections::RegisterNodeConnection(*state,
                                                  3,
                                                  state->dataLoopNodes->NodeID(3),
                                                  DataLoopNode::ConnectionObjectType::FanOnOff,
                                                  state->dataFans->Fan(1).FanName,
                                                  DataLoopNode::ConnectionType::Outlet,
                                                  NodeInputManager::CompFluidStream::Primary,
                                                  false,
                                                  errFlag);

    // Create HPWH
    state->dataWaterThermalTanks->HPWaterHeater.allocate(1);
    state->dataWaterThermalTanks->getWaterThermalTankInputFlag = false;
    state->dataWaterThermalTanks->numHeatPumpWaterHeater = 1;
    state->dataWaterThermalTanks->HPWaterHeater(1).InletAirConfiguration = WaterThermalTanks::WTTAmbientTemp::ZoneAndOA;
    state->dataWaterThermalTanks->HPWaterHeater(1).FanName = state->dataFans->Fan(1).FanName;
    state->dataWaterThermalTanks->HPWaterHeater(1).FanType = "Fan:OnOff";
    state->dataWaterThermalTanks->HPWaterHeater(1).FanOutletNode = 3;

    // Check validation and expected errors
    ASSERT_THROW(state->afn->validate_distribution(), std::runtime_error);
    std::string const error_string = delimited_string({
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanInletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   ** Severe  ** AirflowNetwork::Solver::validate_distribution: 'SupplyFanOutletNode' is not defined as an AirflowNetwork:Distribution:Node "
        "object.",
        "   **  Fatal  ** AirflowNetwork::Solver::validate_distribution: Program terminates for preceding reason(s).",
        "   ...Summary of Errors that led to program termination:",
        "   ..... Reference severe error count=2",
        "   ..... Last severe error=AirflowNetwork::Solver::validate_distribution: 'SupplyFanOutletNode' is not defined as an "
        "AirflowNetwork:Distribution:Node object.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestIntraZoneLinkageZoneIndex)
{

    // Unit test for #8735

    std::string const idf_objects = delimited_string({

        "Version,",
        "    9.5;                     !- Version Identifier",

        "SimulationControl,",
        "    Yes,                     !- Do Zone Sizing Calculation",
        "    Yes,                     !- Do System Sizing Calculation",
        "    No,                      !- Do Plant Sizing Calculation",
        "    Yes,                     !- Run Simulation for Sizing Periods",
        "    No,                      !- Run Simulation for Weather File Run Periods",
        "    No,                      !- Do HVAC Sizing Simulation for Sizing Periods",
        "    1;                       !- Maximum Number of HVAC Sizing Simulation Passes",

        "Building,",
        "    AirflowNetwork 3 Zone Building,  !- Name",
        "    0,                       !- North Axis {deg}",
        "    Suburbs,                 !- Terrain",
        "    0.0400000,               !- Loads Convergence Tolerance Value {W}",
        "    0.0400000,               !- Temperature Convergence Tolerance Value {deltaC}",
        "    MinimalShadowing,        !- Solar Distribution",
        "    25,                      !- Maximum Number of Warmup Days",
        "    6;                       !- Minimum Number of Warmup Days",

        "SurfaceConvectionAlgorithm:Inside,",
        "    Simple;                  !- Algorithm",

        "SurfaceConvectionAlgorithm:Outside,",
        "    SimpleCombined;          !- Algorithm",

        "HeatBalanceAlgorithm,",
        "    ConductionTransferFunction;  !- Algorithm",

        "ZoneCapacitanceMultiplier:ResearchSpecial,",
        "    Multiplier,              !- Name",
        "    ,                        !- Zone or ZoneList Name",
        "    1.0,                     !- Temperature Capacity Multiplier",
        "    1.0,                     !- Humidity Capacity Multiplier",
        "    1.0,                     !- Carbon Dioxide Capacity Multiplier",
        "    1.0;                     !- Generic Contaminant Capacity Multiplier",

        "Timestep,",
        "    6;                       !- Number of Timesteps per Hour",

        "Site:Location,",
        "    CHICAGO_IL_USA TMY2-94846,  !- Name",
        "    41.78,                   !- Latitude {deg}",
        "    -87.75,                  !- Longitude {deg}",
        "    -6.00,                   !- Time Zone {hr}",
        "    190.00;                  !- Elevation {m}",

        "SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Heating 99% Design Conditions DB,  !- Name",
        "    1,                       !- Month",
        "    21,                      !- Day of Month",
        "    WinterDesignDay,         !- Day Type",
        "    -17.3,                   !- Maximum Dry-Bulb Temperature {C}",
        "    0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    -17.3,                   !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    4.9,                     !- Wind Speed {m/s}",
        "    270,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    0.0;                     !- Sky Clearness",

        "SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Cooling 1% Design Conditions DB/MCWB,  !- Name",
        "    7,                       !- Month",
        "    21,                      !- Day of Month",
        "    SummerDesignDay,         !- Day Type",
        "    31.5,                    !- Maximum Dry-Bulb Temperature {C}",
        "    10.7,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    23.0,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    5.3,                     !- Wind Speed {m/s}",
        "    230,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    1.0;                     !- Sky Clearness",

        "RunPeriod,",
        "    Run Period 1,            !- Name",
        "    7,                       !- Begin Month",
        "    1,                       !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    7,                       !- End Month",
        "    2,                       !- End Day of Month",
        "    ,                        !- End Year",
        "    ,                        !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "Site:GroundTemperature:BuildingSurface,",
        "    20.03,                   !- January Ground Temperature {C}",
        "    20.03,                   !- February Ground Temperature {C}",
        "    20.13,                   !- March Ground Temperature {C}",
        "    20.30,                   !- April Ground Temperature {C}",
        "    20.43,                   !- May Ground Temperature {C}",
        "    20.52,                   !- June Ground Temperature {C}",
        "    20.62,                   !- July Ground Temperature {C}",
        "    20.77,                   !- August Ground Temperature {C}",
        "    20.78,                   !- September Ground Temperature {C}",
        "    20.55,                   !- October Ground Temperature {C}",
        "    20.44,                   !- November Ground Temperature {C}",
        "    20.20;                   !- December Ground Temperature {C}",

        "ScheduleTypeLimits,",
        "    Any Number;              !- Name",

        "ScheduleTypeLimits,",
        "    Fraction,                !- Name",
        "    0.0,                     !- Lower Limit Value",
        "    1.0,                     !- Upper Limit Value",
        "    CONTINUOUS;              !- Numeric Type",

        "ScheduleTypeLimits,",
        "    Temperature,             !- Name",
        "    -60,                     !- Lower Limit Value",
        "    200,                     !- Upper Limit Value",
        "    CONTINUOUS;              !- Numeric Type",

        "ScheduleTypeLimits,",
        "    Control Type,            !- Name",
        "    0,                       !- Lower Limit Value",
        "    4,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "ScheduleTypeLimits,",
        "    HVACTemplate Any Number; !- Name",

        "Schedule:Compact,",
        "    Activity Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    131.8;                   !- Field 4",

        "Schedule:Compact,",
        "    Work Eff Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    0.0;                     !- Field 4",

        "Schedule:Compact,",
        "    Clothing Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    1.0;                     !- Field 4",

        "Schedule:Compact,",
        "    Air Velo Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    0.137;                   !- Field 4",

        "Schedule:Compact,",
        "    WindowVentSched,         !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: Wednesday Thursday Friday Saturday,  !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    21.11,                   !- Field 4",
        "    For: Holiday SummerDesignDay,  !- Field 5",
        "    Until: 24:00,            !- Field 6",
        "    21.11,                   !- Field 7",
        "    For: Sunday Monday Tuesday,  !- Field 8",
        "    Until: 24:00,            !- Field 9",
        "    25.55,                   !- Field 10",
        "    For: WinterDesignDay CustomDay1 CustomDay2,  !- Field 11",
        "    Until: 24:00,            !- Field 12",
        "    25.55;                   !- Field 13",

        "Schedule:Compact,",
        "    OFFICE OCCUPANCY,        !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,             !- Field 3",
        "    0.0,                     !- Field 4",
        "    Until: 7:00,             !- Field 5",
        "    0.10,                    !- Field 6",
        "    Until: 8:00,             !- Field 7",
        "    0.50,                    !- Field 8",
        "    Until: 12:00,            !- Field 9",
        "    1.00,                    !- Field 10",
        "    Until: 13:00,            !- Field 11",
        "    0.50,                    !- Field 12",
        "    Until: 16:00,            !- Field 13",
        "    1.00,                    !- Field 14",
        "    Until: 17:00,            !- Field 15",
        "    0.50,                    !- Field 16",
        "    Until: 18:00,            !- Field 17",
        "    0.10,                    !- Field 18",
        "    Until: 24:00,            !- Field 19",
        "    0.0,                     !- Field 20",
        "    For: AllOtherDays,       !- Field 21",
        "    Until: 24:00,            !- Field 22",
        "    0.0;                     !- Field 23",

        "Schedule:Compact,",
        "    INTERMITTENT,            !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 8:00,             !- Field 3",
        "    0.0,                     !- Field 4",
        "    Until: 18:00,            !- Field 5",
        "    1.00,                    !- Field 6",
        "    Until: 24:00,            !- Field 7",
        "    0.0,                     !- Field 8",
        "    For: AllOtherDays,       !- Field 9",
        "    Until: 24:00,            !- Field 10",
        "    0.0;                     !- Field 11",

        "Schedule:Compact,",
        "    OFFICE LIGHTING,         !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,             !- Field 3",
        "    0.05,                    !- Field 4",
        "    Until: 7:00,             !- Field 5",
        "    0.20,                    !- Field 6",
        "    Until: 17:00,            !- Field 7",
        "    1.00,                    !- Field 8",
        "    Until: 18:00,            !- Field 9",
        "    0.50,                    !- Field 10",
        "    Until: 24:00,            !- Field 11",
        "    0.05,                    !- Field 12",
        "    For: AllOtherDays,       !- Field 13",
        "    Until: 24:00,            !- Field 14",
        "    0.05;                    !- Field 15",

        "Schedule:Compact,",
        "    Htg-SetP-Sch,            !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays CustomDay1 CustomDay2,  !- Field 2",
        "    Until: 6:00,             !- Field 3",
        "    13.0,                    !- Field 4",
        "    Until: 7:00,             !- Field 5",
        "    18.0,                    !- Field 6",
        "    Until: 21:00,            !- Field 7",
        "    23.0,                    !- Field 8",
        "    Until: 24:00,            !- Field 9",
        "    13.0,                    !- Field 10",
        "    For: WeekEnds Holiday,   !- Field 11",
        "    Until: 24:00,            !- Field 12",
        "    13.0,                    !- Field 13",
        "    For: SummerDesignDay,    !- Field 14",
        "    Until: 24:00,            !- Field 15",
        "    13.0,                    !- Field 16",
        "    For: WinterDesignDay,    !- Field 17",
        "    Until: 24:00,            !- Field 18",
        "    23.0;                    !- Field 19",

        "Schedule:Compact,",
        "    Clg-SetP-Sch,            !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays CustomDay1 CustomDay2,  !- Field 2",
        "    Until: 7:00,             !- Field 3",
        "    32.0,                    !- Field 4",
        "    Until: 21:00,            !- Field 5",
        "    24.0,                    !- Field 6",
        "    Until: 24:00,            !- Field 7",
        "    32.0,                    !- Field 8",
        "    For: WeekEnds Holiday,   !- Field 9",
        "    Until: 24:00,            !- Field 10",
        "    32.0,                    !- Field 11",
        "    For: SummerDesignDay,    !- Field 12",
        "    Until: 24:00,            !- Field 13",
        "    24.0,                    !- Field 14",
        "    For: WinterDesignDay,    !- Field 15",
        "    Until: 24:00,            !- Field 16",
        "    32.0;                    !- Field 17",

        "Schedule:Compact,",
        "    HVACTemplate-Always 1,   !- Name",
        "    HVACTemplate Any Number, !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    1;                       !- Field 4",

        "Schedule:Compact,",
        "    HVACTemplate-Always 4,   !- Name",
        "    HVACTemplate Any Number, !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    4;                       !- Field 4",

        "Schedule:Compact,",
        "    HVACTemplate-Always 0,   !- Name",
        "    HVACTemplate Any Number, !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,            !- Field 3",
        "    0;                       !- Field 4",

        "Material,",
        "    A1 - 1 IN STUCCO,        !- Name",
        "    Smooth,                  !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    0.6918309,               !- Conductivity {W/m-K}",
        "    1858.142,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "Material,",
        "    C4 - 4 IN COMMON BRICK,  !- Name",
        "    Rough,                   !- Roughness",
        "    0.1014984,               !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1922.216,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7600000,               !- Solar Absorptance",
        "    0.7600000;               !- Visible Absorptance",

        "Material,",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Name",
        "    Smooth,                  !- Roughness",
        "    1.905E-02,               !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1601.846,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "Material,",
        "    C6 - 8 IN CLAY TILE,     !- Name",
        "    Smooth,                  !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    0.5707605,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.8200000,               !- Solar Absorptance",
        "    0.8200000;               !- Visible Absorptance",

        "Material,",
        "    C10 - 8 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    0.2033016,               !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "Material,",
        "    C10 - 4 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    0.1017016,               !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "Material,",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Name",
        "    Rough,                   !- Roughness",
        "    1.2710161E-02,           !- Thickness {m}",
        "    1.435549,                !- Conductivity {W/m-K}",
        "    881.0155,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5500000,               !- Solar Absorptance",
        "    0.5500000;               !- Visible Absorptance",

        "Material,",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Name",
        "    Rough,                   !- Roughness",
        "    9.5402403E-03,           !- Thickness {m}",
        "    0.1902535,               !- Conductivity {W/m-K}",
        "    1121.292,                !- Density {kg/m3}",
        "    1673.600,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "Material,",
        "    B5 - 1 IN DENSE INSULATION,  !- Name",
        "    VeryRough,               !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    4.3239430E-02,           !- Conductivity {W/m-K}",
        "    91.30524,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.5000000,               !- Solar Absorptance",
        "    0.5000000;               !- Visible Absorptance",

        "Material,",
        "    C12 - 2 IN HW CONCRETE,  !- Name",
        "    MediumRough,             !- Roughness",
        "    5.0901599E-02,           !- Thickness {m}",
        "    1.729577,                !- Conductivity {W/m-K}",
        "    2242.585,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "Material,",
        "    1.375in-Solid-Core,      !- Name",
        "    Smooth,                  !- Roughness",
        "    3.4925E-02,              !- Thickness {m}",
        "    0.1525000,               !- Conductivity {W/m-K}",
        "    614.5000,                !- Density {kg/m3}",
        "    1630.0000,               !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "Material:InfraredTransparent,",
        "    IRTMaterial;             !- Name",

        "WindowMaterial:Glazing,",
        "    WIN-LAY-GLASS-LIGHT,     !- Name",
        "    SpectralAverage,         !- Optical Data Type",
        "    ,                        !- Window Glass Spectral Data Set Name",
        "    0.0025,                  !- Thickness {m}",
        "    0.850,                   !- Solar Transmittance at Normal Incidence",
        "    0.075,                   !- Front Side Solar Reflectance at Normal Incidence",
        "    0.075,                   !- Back Side Solar Reflectance at Normal Incidence",
        "    0.901,                   !- Visible Transmittance at Normal Incidence",
        "    0.081,                   !- Front Side Visible Reflectance at Normal Incidence",
        "    0.081,                   !- Back Side Visible Reflectance at Normal Incidence",
        "    0.0,                     !- Infrared Transmittance at Normal Incidence",
        "    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
        "    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
        "    0.9;                     !- Conductivity {W/m-K}",

        "Construction,",
        "    DOOR-CON,                !- Name",
        "    1.375in-Solid-Core;      !- Outside Layer",

        "Construction,",
        "    EXTWALL80,               !- Name",
        "    A1 - 1 IN STUCCO,        !- Outside Layer",
        "    C4 - 4 IN COMMON BRICK,  !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "Construction,",
        "    PARTITION06,             !- Name",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD,  !- Outside Layer",
        "    C6 - 8 IN CLAY TILE,     !- Layer 2",
        "    E1 - 3 / 4 IN PLASTER OR GYP BOARD;  !- Layer 3",

        "Construction,",
        "    FLOOR SLAB 8 IN,         !- Name",
        "    C10 - 8 IN HW CONCRETE;  !- Outside Layer",

        "Construction,",
        "    FLOOR SLAB 4 IN,         !- Name",
        "    C10 - 4 IN HW CONCRETE;  !- Outside Layer",

        "Construction,",
        "    ROOF34,                  !- Name",
        "    E2 - 1 / 2 IN SLAG OR STONE,  !- Outside Layer",
        "    E3 - 3 / 8 IN FELT AND MEMBRANE,  !- Layer 2",
        "    B5 - 1 IN DENSE INSULATION,  !- Layer 3",
        "    C12 - 2 IN HW CONCRETE;  !- Layer 4",

        "Construction,",
        "    WIN-CON-LIGHT,           !- Name",
        "    WIN-LAY-GLASS-LIGHT;     !- Outside Layer",

        "GlobalGeometryRules,",
        "    UpperLeftCorner,         !- Starting Vertex Position",
        "    CounterClockWise,        !- Vertex Entry Direction",
        "    World;                   !- Coordinate System",

        "Zone,",
        "    WEST_ZONE,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate;           !- Ceiling Height {m}",

        "Zone,",
        "    EAST_ZONE,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate;           !- Ceiling Height {m}",

        "Zone,",
        "    NORTH_ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate;           !- Ceiling Height {m}",

        "Zone,",
        "    WEST_ZONE_T,             !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate;           !- Ceiling Height {m}",

        "Zone,",
        "    EAST_ZONE_T,             !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate;           !- Ceiling Height {m}",

        "BuildingSurface:Detailed,",
        "    Surface_1,               !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    WEST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_1_T,             !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    WEST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_2,               !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    WEST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_2_T,             !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    WEST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_3,               !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    WEST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_17,              !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_3_T,             !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    WEST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_17_T,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_4,               !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    WEST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_10,              !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_4_T,             !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    WEST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_10_T,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_5,               !- Name",
        "    FLOOR,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    WEST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_5,               !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    0,                       !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    0;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_5_T,             !- Name",
        "    FLOOR,                   !- Surface Type",
        "    FLOOR SLAB 4 IN,         !- Construction Name",
        "    WEST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Zone,                    !- Outside Boundary Condition",
        "    WEST_ZONE,               !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    3.048,                   !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    3.048;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_6,               !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    WEST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    6.096,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    6.096,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_8,               !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_8_T,             !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_9,               !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_9_T,             !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    EAST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_10,              !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_4,               !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    6.096001,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_10_T,            !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_4_T,             !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096001,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_11,              !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_18,              !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_11_T,            !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    EAST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_18_T,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096000,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096000,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_12,              !- Name",
        "    FLOOR,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    EAST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_12,              !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    0,                       !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    0;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_12_T,            !- Name",
        "    FLOOR,                   !- Surface Type",
        "    FLOOR SLAB 4 IN,         !- Construction Name",
        "    EAST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Zone,                    !- Outside Boundary Condition",
        "    EAST_ZONE,               !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    3.048,                   !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    3.048;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_13,              !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    EAST_ZONE_T,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    6.096,                   !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    6.096,                   !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_14,              !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    12.19200,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    12.19200,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_14_T,            !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    12.19200,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    12.19200,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_15,              !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    12.19200,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    12.19200,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    12.19200,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    12.19200,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_15_T,            !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    12.19200,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    12.19200,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    0,                       !- Vertex 3 X-coordinate {m}",
        "    12.19200,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    0,                       !- Vertex 4 X-coordinate {m}",
        "    12.19200,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_16,              !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    12.19200,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    12.19200,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_16_T,            !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL80,               !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    12.19200,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    12.19200,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    12.19200,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    12.19200,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_17,              !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_3,               !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0.000,                   !- Vertex 1 X-coordinate {m}",
        "    6.096,                   !- Vertex 1 Y-coordinate {m}",
        "    3.048,                   !- Vertex 1 Z-coordinate {m}",
        "    0.000,                   !- Vertex 2 X-coordinate {m}",
        "    6.096,                   !- Vertex 2 Y-coordinate {m}",
        "    0.000,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096,                   !- Vertex 3 X-coordinate {m}",
        "    6.096,                   !- Vertex 3 Y-coordinate {m}",
        "    0.000,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096,                   !- Vertex 4 X-coordinate {m}",
        "    6.096,                   !- Vertex 4 Y-coordinate {m}",
        "    3.048;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_17_T,            !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_3_T,             !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0.000,                   !- Vertex 1 X-coordinate {m}",
        "    6.096,                   !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    0.000,                   !- Vertex 2 X-coordinate {m}",
        "    6.096,                   !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    6.096,                   !- Vertex 3 X-coordinate {m}",
        "    6.096,                   !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    6.096,                   !- Vertex 4 X-coordinate {m}",
        "    6.096,                   !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_18_T,            !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_11_T,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_18,              !- Name",
        "    WALL,                    !- Surface Type",
        "    PARTITION06,             !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_11,              !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,                !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    3.048000,                !- Vertex 1 Z-coordinate {m}",
        "    6.096000,                !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    3.048000;                !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_19,              !- Name",
        "    FLOOR,                   !- Surface Type",
        "    FLOOR SLAB 8 IN,         !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Surface_19,              !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    1.000000,                !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    0,                       !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    12.19200,                !- Vertex 2 Y-coordinate {m}",
        "    0,                       !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    12.19200,                !- Vertex 3 Y-coordinate {m}",
        "    0,                       !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    0;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "    Surface_20,              !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF34,                  !- Construction Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,                       !- Vertex 1 X-coordinate {m}",
        "    12.19200,                !- Vertex 1 Y-coordinate {m}",
        "    6.096,                   !- Vertex 1 Z-coordinate {m}",
        "    0,                       !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    6.096,                   !- Vertex 2 Z-coordinate {m}",
        "    12.19200,                !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    6.096,                   !- Vertex 3 Z-coordinate {m}",
        "    12.19200,                !- Vertex 4 X-coordinate {m}",
        "    12.19200,                !- Vertex 4 Y-coordinate {m}",
        "    6.096;                   !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    WINDOW1,                 !- Name",
        "    WINDOW,                  !- Surface Type",
        "    WIN-CON-LIGHT,           !- Construction Name",
        "    Surface_1,               !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    1.00000,                 !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    2.500000,                !- Vertex 1 Z-coordinate {m}",
        "    1.00000,                 !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    1.0000000,               !- Vertex 2 Z-coordinate {m}",
        "    5.000000,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    1.0000000,               !- Vertex 3 Z-coordinate {m}",
        "    5.000000,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    2.500000;                !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    DoorInSurface_3,         !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Surface_3,               !- Building Surface Name",
        "    DoorInSurface_17,        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    3.500,                   !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    2.0,                     !- Vertex 1 Z-coordinate {m}",
        "    3.500,                   !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0.0,                     !- Vertex 2 Z-coordinate {m}",
        "    2.500,                   !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0.0,                     !- Vertex 3 Z-coordinate {m}",
        "    2.500,                   !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    2.0;                     !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    WINDOW2,                 !- Name",
        "    WINDOW,                  !- Surface Type",
        "    WIN-CON-LIGHT,           !- Construction Name",
        "    Surface_15,              !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    6.000000,                !- Vertex 1 X-coordinate {m}",
        "    12.19200,                !- Vertex 1 Y-coordinate {m}",
        "    2.333000,                !- Vertex 1 Z-coordinate {m}",
        "    6.000000,                !- Vertex 2 X-coordinate {m}",
        "    12.19200,                !- Vertex 2 Y-coordinate {m}",
        "    1.000000,                !- Vertex 2 Z-coordinate {m}",
        "    3.000000,                !- Vertex 3 X-coordinate {m}",
        "    12.19200,                !- Vertex 3 Y-coordinate {m}",
        "    1.000000,                !- Vertex 3 Z-coordinate {m}",
        "    3.000000,                !- Vertex 4 X-coordinate {m}",
        "    12.19200,                !- Vertex 4 Y-coordinate {m}",
        "    2.333000;                !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    DoorInSurface_17,        !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Surface_17,              !- Building Surface Name",
        "    DoorInSurface_3,         !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    2.500,                   !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    2.0,                     !- Vertex 1 Z-coordinate {m}",
        "    2.500,                   !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    0.0,                     !- Vertex 2 Z-coordinate {m}",
        "    3.500,                   !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    0.0,                     !- Vertex 3 Z-coordinate {m}",
        "    3.500,                   !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    2.0;                     !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    WINDOW1_T,               !- Name",
        "    WINDOW,                  !- Surface Type",
        "    WIN-CON-LIGHT,           !- Construction Name",
        "    Surface_1_T,             !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    1.00000,                 !- Vertex 1 X-coordinate {m}",
        "    0,                       !- Vertex 1 Y-coordinate {m}",
        "    5.548,                   !- Vertex 1 Z-coordinate {m}",
        "    1.00000,                 !- Vertex 2 X-coordinate {m}",
        "    0,                       !- Vertex 2 Y-coordinate {m}",
        "    4.048,                   !- Vertex 2 Z-coordinate {m}",
        "    5.000000,                !- Vertex 3 X-coordinate {m}",
        "    0,                       !- Vertex 3 Y-coordinate {m}",
        "    4.048,                   !- Vertex 3 Z-coordinate {m}",
        "    5.000000,                !- Vertex 4 X-coordinate {m}",
        "    0,                       !- Vertex 4 Y-coordinate {m}",
        "    5.548;                   !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    DoorInSurface_3_T,       !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Surface_3_T,             !- Building Surface Name",
        "    DoorInSurface_17_T,      !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    3.500,                   !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    5.048,                   !- Vertex 1 Z-coordinate {m}",
        "    3.500,                   !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    2.500,                   !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    2.500,                   !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    5.048;                   !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    WINDOW2_T,               !- Name",
        "    WINDOW,                  !- Surface Type",
        "    WIN-CON-LIGHT,           !- Construction Name",
        "    Surface_15_T,            !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    6.000000,                !- Vertex 1 X-coordinate {m}",
        "    12.19200,                !- Vertex 1 Y-coordinate {m}",
        "    5.381,                   !- Vertex 1 Z-coordinate {m}",
        "    6.000000,                !- Vertex 2 X-coordinate {m}",
        "    12.19200,                !- Vertex 2 Y-coordinate {m}",
        "    4.048,                   !- Vertex 2 Z-coordinate {m}",
        "    3.000000,                !- Vertex 3 X-coordinate {m}",
        "    12.19200,                !- Vertex 3 Y-coordinate {m}",
        "    4.048,                   !- Vertex 3 Z-coordinate {m}",
        "    3.000000,                !- Vertex 4 X-coordinate {m}",
        "    12.19200,                !- Vertex 4 Y-coordinate {m}",
        "    5.381;                   !- Vertex 4 Z-coordinate {m}",

        "FenestrationSurface:Detailed,",
        "    DoorInSurface_17_T,      !- Name",
        "    DOOR,                    !- Surface Type",
        "    DOOR-CON,                !- Construction Name",
        "    Surface_17_T,            !- Building Surface Name",
        "    DoorInSurface_3_T,       !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    2.500,                   !- Vertex 1 X-coordinate {m}",
        "    6.096000,                !- Vertex 1 Y-coordinate {m}",
        "    5.048,                   !- Vertex 1 Z-coordinate {m}",
        "    2.500,                   !- Vertex 2 X-coordinate {m}",
        "    6.096000,                !- Vertex 2 Y-coordinate {m}",
        "    3.048,                   !- Vertex 2 Z-coordinate {m}",
        "    3.500,                   !- Vertex 3 X-coordinate {m}",
        "    6.096000,                !- Vertex 3 Y-coordinate {m}",
        "    3.048,                   !- Vertex 3 Z-coordinate {m}",
        "    3.500,                   !- Vertex 4 X-coordinate {m}",
        "    6.096000,                !- Vertex 4 Y-coordinate {m}",
        "    5.048;                   !- Vertex 4 Z-coordinate {m}",

        "RoomAirModelType,",
        "    RoomAirWithAirflowNetwork,  !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    AirflowNetwork,          !- Room-Air Modeling Type",
        "    DIRECT;                  !- Air Temperature Coupling Strategy",

        "RoomAir:Node:AirflowNetwork,",
        "    LeftUpper,               !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    0.15,                    !- Fraction of Zone Air Volume",
        "    Surface_18_T_List,       !- RoomAir:Node:AirflowNetwork:AdjacentSurfaceList Name",
        "    LeftUpper_Gain,          !- RoomAir:Node:AirflowNetwork:InternalGains Name",
        "    LeftUpper_HVAC;          !- RoomAir:Node:AirflowNetwork:HVACEquipment Name",

        "RoomAir:Node:AirflowNetwork,",
        "    CentralUpper,            !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    0.15,                    !- Fraction of Zone Air Volume",
        "    Surface_20_List,         !- RoomAir:Node:AirflowNetwork:AdjacentSurfaceList Name",
        "    CentralUpper_Gain,       !- RoomAir:Node:AirflowNetwork:InternalGains Name",
        "    CentralUpper_HVAC;       !- RoomAir:Node:AirflowNetwork:HVACEquipment Name",

        "RoomAir:Node:AirflowNetwork,",
        "    LeftMiddle,              !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    0.15,                    !- Fraction of Zone Air Volume",
        "    Surface_14_List,         !- RoomAir:Node:AirflowNetwork:AdjacentSurfaceList Name",
        "    LeftMiddle_Gain,         !- RoomAir:Node:AirflowNetwork:InternalGains Name",
        "    LeftMiddle_HVAC;         !- RoomAir:Node:AirflowNetwork:HVACEquipment Name",

        "RoomAir:Node:AirflowNetwork,",
        "    North_Zone,              !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    0.25,                    !- Fraction of Zone Air Volume",
        "    Surface_16_List,         !- RoomAir:Node:AirflowNetwork:AdjacentSurfaceList Name",
        "    NORTH_ZONE_Gain,         !- RoomAir:Node:AirflowNetwork:InternalGains Name",
        "    NORTH_ZONE_HVAC;         !- RoomAir:Node:AirflowNetwork:HVACEquipment Name",

        "RoomAir:Node:AirflowNetwork,",
        "    LeftLower,               !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    0.15,                    !- Fraction of Zone Air Volume",
        "    Surface_18_List,         !- RoomAir:Node:AirflowNetwork:AdjacentSurfaceList Name",
        "    LeftLower_Gain,          !- RoomAir:Node:AirflowNetwork:InternalGains Name",
        "    LeftLower_HVAC;          !- RoomAir:Node:AirflowNetwork:HVACEquipment Name",

        "RoomAir:Node:AirflowNetwork,",
        "    CentralLower,            !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    0.15,                    !- Fraction of Zone Air Volume",
        "    Surface_19_List,         !- RoomAir:Node:AirflowNetwork:AdjacentSurfaceList Name",
        "    CentralLower_Gain,       !- RoomAir:Node:AirflowNetwork:InternalGains Name",
        "    CentralLower_HVAC;       !- RoomAir:Node:AirflowNetwork:HVACEquipment Name",

        "RoomAir:Node:AirflowNetwork:AdjacentSurfaceList,",
        "    Surface_18_T_List,       !- Name",
        "    Surface_18_T;            !- Surface 1 Name",

        "RoomAir:Node:AirflowNetwork:AdjacentSurfaceList,",
        "    Surface_20_List,         !- Name",
        "    Surface_20;              !- Surface 1 Name",

        "RoomAir:Node:AirflowNetwork:AdjacentSurfaceList,",
        "    Surface_14_List,         !- Name",
        "    Surface_14;              !- Surface 1 Name",

        "RoomAir:Node:AirflowNetwork:AdjacentSurfaceList,",
        "    Surface_16_List,         !- Name",
        "    Surface_16;              !- Surface 1 Name",

        "RoomAir:Node:AirflowNetwork:AdjacentSurfaceList,",
        "    Surface_18_List,         !- Name",
        "    Surface_18;              !- Surface 1 Name",

        "RoomAir:Node:AirflowNetwork:AdjacentSurfaceList,",
        "    Surface_19_List,         !- Name",
        "    Surface_19;              !- Surface 1 Name",

        "RoomAir:Node:AirflowNetwork:InternalGains,",
        "    LeftUpper_Gain,          !- Name",
        "    People,                  !- Internal Gain Object 1 Type",
        "    NORTH_ZONE People,       !- Internal Gain Object 1 Name",
        "    0.15,                    !- Fraction of Gains to Node 1",
        "    Lights,                  !- Internal Gain Object 2 Type",
        "    NORTH_ZONE Lights,       !- Internal Gain Object 2 Name",
        "    0.15,                    !- Fraction of Gains to Node 2",
        "    ElectricEquipment,       !- Internal Gain Object 3 Type",
        "    NORTH_ZONE Equip,        !- Internal Gain Object 3 Name",
        "    0.15;                    !- Fraction of Gains to Node 3",

        "RoomAir:Node:AirflowNetwork:InternalGains,",
        "    CentralUpper_Gain,       !- Name",
        "    People,                  !- Internal Gain Object 1 Type",
        "    NORTH_ZONE People,       !- Internal Gain Object 1 Name",
        "    0.15,                    !- Fraction of Gains to Node 1",
        "    Lights,                  !- Internal Gain Object 2 Type",
        "    NORTH_ZONE Lights,       !- Internal Gain Object 2 Name",
        "    0.15,                    !- Fraction of Gains to Node 2",
        "    ElectricEquipment,       !- Internal Gain Object 3 Type",
        "    NORTH_ZONE Equip,        !- Internal Gain Object 3 Name",
        "    0.15;                    !- Fraction of Gains to Node 3",

        "RoomAir:Node:AirflowNetwork:InternalGains,",
        "    LeftMiddle_Gain,         !- Name",
        "    People,                  !- Internal Gain Object 1 Type",
        "    NORTH_ZONE People,       !- Internal Gain Object 1 Name",
        "    0.15,                    !- Fraction of Gains to Node 1",
        "    Lights,                  !- Internal Gain Object 2 Type",
        "    NORTH_ZONE Lights,       !- Internal Gain Object 2 Name",
        "    0.15,                    !- Fraction of Gains to Node 2",
        "    ElectricEquipment,       !- Internal Gain Object 3 Type",
        "    NORTH_ZONE Equip,        !- Internal Gain Object 3 Name",
        "    0.15;                    !- Fraction of Gains to Node 3",

        "RoomAir:Node:AirflowNetwork:InternalGains,",
        "    NORTH_ZONE_Gain,         !- Name",
        "    People,                  !- Internal Gain Object 1 Type",
        "    NORTH_ZONE People,       !- Internal Gain Object 1 Name",
        "    0.20,                    !- Fraction of Gains to Node 1",
        "    Lights,                  !- Internal Gain Object 2 Type",
        "    NORTH_ZONE Lights,       !- Internal Gain Object 2 Name",
        "    0.20,                    !- Fraction of Gains to Node 2",
        "    ElectricEquipment,       !- Internal Gain Object 3 Type",
        "    NORTH_ZONE Equip,        !- Internal Gain Object 3 Name",
        "    0.20;                    !- Fraction of Gains to Node 3",

        "RoomAir:Node:AirflowNetwork:InternalGains,",
        "    LeftLower_Gain,          !- Name",
        "    People,                  !- Internal Gain Object 1 Type",
        "    NORTH_ZONE People,       !- Internal Gain Object 1 Name",
        "    0.15,                    !- Fraction of Gains to Node 1",
        "    Lights,                  !- Internal Gain Object 2 Type",
        "    NORTH_ZONE Lights,       !- Internal Gain Object 2 Name",
        "    0.15,                    !- Fraction of Gains to Node 2",
        "    ElectricEquipment,       !- Internal Gain Object 3 Type",
        "    NORTH_ZONE Equip,        !- Internal Gain Object 3 Name",
        "    0.15;                    !- Fraction of Gains to Node 3",

        "RoomAir:Node:AirflowNetwork:InternalGains,",
        "    CentralLower_Gain,       !- Name",
        "    People,                  !- Internal Gain Object 1 Type",
        "    NORTH_ZONE People,       !- Internal Gain Object 1 Name",
        "    0.20,                    !- Fraction of Gains to Node 1",
        "    Lights,                  !- Internal Gain Object 2 Type",
        "    NORTH_ZONE Lights,       !- Internal Gain Object 2 Name",
        "    0.20,                    !- Fraction of Gains to Node 2",
        "    ElectricEquipment,       !- Internal Gain Object 3 Type",
        "    NORTH_ZONE Equip,        !- Internal Gain Object 3 Name",
        "    0.20;                    !- Fraction of Gains to Node 3",

        "RoomAir:Node:AirflowNetwork:HVACEquipment,",
        "    LeftUpper_HVAC,          !- Name",
        "    ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC or Air Terminal Equipment Object Type 1",
        "    NORTH_ZONE PTAC,         !- ZoneHVAC or Air Terminal Equipment Object Name 1",
        "    0.04,                    !- Fraction of Output or Supply Air from HVAC Equipment 1",
        "    0.04;                    !- Fraction of Input or Return Air to HVAC Equipment 1",

        "RoomAir:Node:AirflowNetwork:HVACEquipment,",
        "    CentralUpper_HVAC,       !- Name",
        "    ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC or Air Terminal Equipment Object Type 1",
        "    NORTH_ZONE PTAC,         !- ZoneHVAC or Air Terminal Equipment Object Name 1",
        "    0.14,                    !- Fraction of Output or Supply Air from HVAC Equipment 1",
        "    0.14;                    !- Fraction of Input or Return Air to HVAC Equipment 1",

        "RoomAir:Node:AirflowNetwork:HVACEquipment,",
        "    LeftMiddle_HVAC,         !- Name",
        "    ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC or Air Terminal Equipment Object Type 1",
        "    NORTH_ZONE PTAC,         !- ZoneHVAC or Air Terminal Equipment Object Name 1",
        "    0.04,                    !- Fraction of Output or Supply Air from HVAC Equipment 1",
        "    0.04;                    !- Fraction of Input or Return Air to HVAC Equipment 1",

        "RoomAir:Node:AirflowNetwork:HVACEquipment,",
        "    NORTH_ZONE_HVAC,         !- Name",
        "    ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC or Air Terminal Equipment Object Type 1",
        "    NORTH_ZONE PTAC,         !- ZoneHVAC or Air Terminal Equipment Object Name 1",
        "    0.68,                    !- Fraction of Output or Supply Air from HVAC Equipment 1",
        "    0.68;                    !- Fraction of Input or Return Air to HVAC Equipment 1",

        "RoomAir:Node:AirflowNetwork:HVACEquipment,",
        "    LeftLower_HVAC,          !- Name",
        "    ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC or Air Terminal Equipment Object Type 1",
        "    NORTH_ZONE PTAC,         !- ZoneHVAC or Air Terminal Equipment Object Name 1",
        "    0.06,                    !- Fraction of Output or Supply Air from HVAC Equipment 1",
        "    0.06;                    !- Fraction of Input or Return Air to HVAC Equipment 1",

        "RoomAir:Node:AirflowNetwork:HVACEquipment,",
        "    CentralLower_HVAC,       !- Name",
        "    ZoneHVAC:PackagedTerminalAirConditioner,  !- ZoneHVAC or Air Terminal Equipment Object Type 1",
        "    NORTH_ZONE PTAC,         !- ZoneHVAC or Air Terminal Equipment Object Name 1",
        "    0.04,                    !- Fraction of Output or Supply Air from HVAC Equipment 1",
        "    0.04;                    !- Fraction of Input or Return Air to HVAC Equipment 1",

        "RoomAirSettings:AirflowNetwork,",
        "    NORTH_ZONE,              !- Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    North_Zone,              !- Control Point RoomAirflowNetwork:Node Name",
        "    LeftUpper,               !- RoomAirflowNetwork:Node Name 1",
        "    CentralUpper,            !- RoomAirflowNetwork:Node Name 2",
        "    North_Zone,              !- RoomAirflowNetwork:Node Name 3",
        "    LeftMiddle,              !- RoomAirflowNetwork:Node Name 4",
        "    LeftLower,               !- RoomAirflowNetwork:Node Name 5",
        "    CentralLower;            !- RoomAirflowNetwork:Node Name 6",

        "People,",
        "    NORTH_ZONE People,       !- Name",
        "    NORTH_ZONE,              !- Zone or ZoneList Name",
        "    OFFICE OCCUPANCY,        !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    4.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "Lights,",
        "    EAST_ZONE Lights,        !- Name",
        "    EAST_ZONE,               !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    0,                       !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "Lights,",
        "    NORTH_ZONE Lights,       !- Name",
        "    NORTH_ZONE,              !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    878.6252,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    0,                       !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "Lights,",
        "    EAST_ZONE_T Lights,      !- Name",
        "    EAST_ZONE_T,             !- Zone or ZoneList Name",
        "    OFFICE LIGHTING,         !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1464.375,                !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    0,                       !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "ElectricEquipment,",
        "    WEST_ZONE Equip,         !- Name",
        "    WEST_ZONE,               !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    2928.751,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "ElectricEquipment,",
        "    EAST_ZONE Equip,         !- Name",
        "    EAST_ZONE,               !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    1464.375,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "ElectricEquipment,",
        "    NORTH_ZONE Equip,        !- Name",
        "    NORTH_ZONE,              !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    2928.751,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "ElectricEquipment,",
        "    WEST_ZONE_T Equip,       !- Name",
        "    WEST_ZONE_T,             !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    2928.751,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "ElectricEquipment,",
        "    EAST_ZONE_T Equip,       !- Name",
        "    EAST_ZONE_T,             !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    1464.375,                !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "AirflowNetwork:SimulationControl,",
        "    NaturalVentilation,      !- Name",
        "    MultizoneWithoutDistribution,  !- AirflowNetwork Control",
        "    INPUT,                   !- Wind Pressure Coefficient Type",
        "    ExternalNode,            !- Height Selection for Local Wind Pressure Calculation",
        "    LOWRISE,                 !- Building Type",
        "    500,                     !- Maximum Number of Iterations {dimensionless}",
        "    ZeroNodePressures,       !- Initialization Type",
        "    1.0E-05,                 !- Relative Airflow Convergence Tolerance {dimensionless}",
        "    1.0E-06,                 !- Absolute Airflow Convergence Tolerance {kg/s}",
        "    -0.5,                    !- Convergence Acceleration Limit {dimensionless}",
        "    0.0,                     !- Azimuth Angle of Long Axis of Building {deg}",
        "    1.0;                     !- Ratio of Building Width Along Short Axis to Width Along Long Axis",

        "AirflowNetwork:MultiZone:Zone,",
        "    NORTH_ZONE,              !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Surface_14,              !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Surface_15,              !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Window2,                 !- Surface Name",
        "    WiOpen1,                 !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    0.01;                    !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Surface_16,              !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    EFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Surface_14_T,            !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Surface_15_T,            !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Window2_T,               !- Surface Name",
        "    WiOpen1,                 !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    0.01;                    !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:Surface,",
        "    Surface_16_T,            !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    EFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "    ReferenceCrackConditions,!- Name",
        "    20.0,                    !- Reference Temperature {C}",
        "    101320,                  !- Reference Barometric Pressure {Pa}",
        "    0.005;                   !- Reference Humidity Ratio {kgWater/kgDryAir}",

        "AirflowNetwork:MultiZone:Surface:Crack,",
        "    CR-1,                    !- Name",
        "    0.01,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667,                   !- Air Mass Flow Exponent {dimensionless}",
        "    ReferenceCrackConditions;!- Reference Crack Conditions",

        "AirflowNetwork:MultiZone:Surface:Crack,",
        "    CRcri,                   !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667,                   !- Air Mass Flow Exponent {dimensionless}",
        "    ReferenceCrackConditions;!- Reference Crack Conditions",

        "AirflowNetwork:MultiZone:Component:DetailedOpening,",
        "    WiOpen1,                 !- Name",
        "    0.001,                   !- Air Mass Flow Coefficient When Opening is Closed {kg/s-m}",
        "    0.667,                   !- Air Mass Flow Exponent When Opening is Closed {dimensionless}",
        "    NonPivoted,              !- Type of Rectangular Large Vertical Opening (LVO)",
        "    0.0,                     !- Extra Crack Length or Height of Pivoting Axis {m}",
        "    2,                       !- Number of Sets of Opening Factor Data",
        "    0.0,                     !- Opening Factor 1 {dimensionless}",
        "    0.5,                     !- Discharge Coefficient for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Width Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Opening Factor 2 {dimensionless}",
        "    0.6,                     !- Discharge Coefficient for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Width Factor for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 2 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 2 {dimensionless}",
        "    0,                       !- Opening Factor 3 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 3 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Start Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Opening Factor 4 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 4 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 4 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 4 {dimensionless}",
        "    0;                       !- Start Height Factor for Opening Factor 4 {dimensionless}",

        "AirflowNetwork:MultiZone:Component:DetailedOpening,",
        "    DrOpen,                  !- Name",
        "    0.001,                   !- Air Mass Flow Coefficient When Opening is Closed {kg/s-m}",
        "    0.667,                   !- Air Mass Flow Exponent When Opening is Closed {dimensionless}",
        "    NonPivoted,              !- Type of Rectangular Large Vertical Opening (LVO)",
        "    0.0,                     !- Extra Crack Length or Height of Pivoting Axis {m}",
        "    2,                       !- Number of Sets of Opening Factor Data",
        "    0.0,                     !- Opening Factor 1 {dimensionless}",
        "    0.5,                     !- Discharge Coefficient for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Width Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 1 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 1 {dimensionless}",
        "    1.0,                     !- Opening Factor 2 {dimensionless}",
        "    0.6,                     !- Discharge Coefficient for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Width Factor for Opening Factor 2 {dimensionless}",
        "    1.0,                     !- Height Factor for Opening Factor 2 {dimensionless}",
        "    0.0,                     !- Start Height Factor for Opening Factor 2 {dimensionless}",
        "    0,                       !- Opening Factor 3 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 3 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Start Height Factor for Opening Factor 3 {dimensionless}",
        "    0,                       !- Opening Factor 4 {dimensionless}",
        "    0,                       !- Discharge Coefficient for Opening Factor 4 {dimensionless}",
        "    0,                       !- Width Factor for Opening Factor 4 {dimensionless}",
        "    0,                       !- Height Factor for Opening Factor 4 {dimensionless}",
        "    0;                       !- Start Height Factor for Opening Factor 4 {dimensionless}",

        "AirflowNetwork:MultiZone:Component:HorizontalOpening,",
        "    HrOpen,                  !- Name",
        "    0.001,                   !- Air Mass Flow Coefficient When Opening is Closed {kg/s-m}",
        "    0.667,                   !- Air Mass Flow Exponent When Opening is Closed {dimensionless}",
        "    90.0,                    !- Sloping Plane Angle {deg}",
        "    0.2;                     !- Discharge Coefficient {dimensionless}",

        "AirflowNetwork:MultiZone:ExternalNode,",
        "    NFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    NFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "AirflowNetwork:MultiZone:ExternalNode,",
        "    EFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    EFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "AirflowNetwork:MultiZone:ExternalNode,",
        "    SFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    SFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "AirflowNetwork:MultiZone:ExternalNode,",
        "    WFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    WFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "AirflowNetwork:MultiZone:WindPressureCoefficientArray,",
        "    Every 30 Degrees,        !- Name",
        "    0,                       !- Wind Direction 1 {deg}",
        "    30,                      !- Wind Direction 2 {deg}",
        "    60,                      !- Wind Direction 3 {deg}",
        "    90,                      !- Wind Direction 4 {deg}",
        "    120,                     !- Wind Direction 5 {deg}",
        "    150,                     !- Wind Direction 6 {deg}",
        "    180,                     !- Wind Direction 7 {deg}",
        "    210,                     !- Wind Direction 8 {deg}",
        "    240,                     !- Wind Direction 9 {deg}",
        "    270,                     !- Wind Direction 10 {deg}",
        "    300,                     !- Wind Direction 11 {deg}",
        "    330;                     !- Wind Direction 12 {deg}",

        "AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    NFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.60,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.48;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    EFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.56;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    SFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.37,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.42;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    WFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.04;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "AirflowNetwork:IntraZone:Node,",
        "    LeftUpper,               !- Name",
        "    LeftUpper,               !- RoomAir:Node:AirflowNetwork Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    4.572;                   !- Node Height {m}",

        "AirflowNetwork:IntraZone:Node,",
        "    CentralUpper,            !- Name",
        "    CentralUpper,            !- RoomAir:Node:AirflowNetwork Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    4.572;                   !- Node Height {m}",

        "AirflowNetwork:IntraZone:Node,",
        "    LeftMiddle,              !- Name",
        "    LeftMiddle,              !- RoomAir:Node:AirflowNetwork Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    3.048;                   !- Node Height {m}",

        "AirflowNetwork:IntraZone:Node,",
        "    LeftLower,               !- Name",
        "    LeftLower,               !- RoomAir:Node:AirflowNetwork Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    1.524;                   !- Node Height {m}",

        "AirflowNetwork:IntraZone:Node,",
        "    CentralLower,            !- Name",
        "    CentralLower,            !- RoomAir:Node:AirflowNetwork Name",
        "    NORTH_ZONE,              !- Zone Name",
        "    1.524;                   !- Node Height {m}",

        "AirflowNetwork:IntraZone:Linkage,",
        "    Link1,                   !- Name",
        "    LeftUpper,               !- Node 1 Name",
        "    CentralUpper,            !- Node 2 Name",
        "    CR-1;                    !- Component Name",

        "AirflowNetwork:IntraZone:Linkage,",
        "    Link2,                   !- Name",
        "    LeftUpper,               !- Node 1 Name",
        "    LeftMiddle,              !- Node 2 Name",
        "    CR-1;                    !- Component Name",

        "AirflowNetwork:IntraZone:Linkage,",
        "    Link3,                   !- Name",
        "    LeftLower,               !- Node 1 Name",
        "    LeftMiddle,              !- Node 2 Name",
        "    CR-1;                    !- Component Name",

        "AirflowNetwork:IntraZone:Linkage,",
        "    Link4,                   !- Name",
        "    LeftLower,               !- Node 1 Name",
        "    CentralLower,            !- Node 2 Name",
        "    CR-1;                    !- Component Name",

        "AirflowNetwork:IntraZone:Linkage,",
        "    Link5,                   !- Name",
        "    CentralUpper,            !- Node 1 Name",
        "    North_Zone,              !- Node 2 Name",
        "    CR-1;                    !- Component Name",

        "AirflowNetwork:IntraZone:Linkage,",
        "    Link6,                   !- Name",
        "    CentralLower,            !- Node 1 Name",
        "    North_Zone,              !- Node 2 Name",
        "    CR-1;                    !- Component Name",

        "DesignSpecification:OutdoorAir,",
        "    SZ DSOA NORTH_ZONE,      !- Name",
        "    flow/person,             !- Outdoor Air Method",
        "    0.00944,                 !- Outdoor Air Flow per Person {m3/s-person}",
        "    0.0,                     !- Outdoor Air Flow per Zone Floor Area {m3/s-m2}",
        "    0.0;                     !- Outdoor Air Flow per Zone {m3/s}",

        "DesignSpecification:ZoneAirDistribution,",
        "    SZ DSZAD NORTH_ZONE,     !- Name",
        "    1,                       !- Zone Air Distribution Effectiveness in Cooling Mode {dimensionless}",
        "    1;                       !- Zone Air Distribution Effectiveness in Heating Mode {dimensionless}",

        "Sizing:Parameters,",
        "    1.2,                     !- Heating Sizing Factor",
        "    1.2;                     !- Cooling Sizing Factor",

        "Sizing:Zone,",
        "    NORTH_ZONE,              !- Zone or ZoneList Name",
        "    SupplyAirTemperature,    !- Zone Cooling Design Supply Air Temperature Input Method",
        "    14.0,                    !- Zone Cooling Design Supply Air Temperature {C}",
        "    11.11,                   !- Zone Cooling Design Supply Air Temperature Difference {deltaC}",
        "    SupplyAirTemperature,    !- Zone Heating Design Supply Air Temperature Input Method",
        "    50.0,                    !- Zone Heating Design Supply Air Temperature {C}",
        "    ,                        !- Zone Heating Design Supply Air Temperature Difference {deltaC}",
        "    0.008,                   !- Zone Cooling Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "    0.008,                   !- Zone Heating Design Supply Air Humidity Ratio {kgWater/kgDryAir}",
        "    SZ DSOA NORTH_ZONE,      !- Design Specification Outdoor Air Object Name",
        "    ,                        !- Zone Heating Sizing Factor",
        "    ,                        !- Zone Cooling Sizing Factor",
        "    DesignDay,               !- Cooling Design Air Flow Method",
        "    0,                       !- Cooling Design Air Flow Rate {m3/s}",
        "    ,                        !- Cooling Minimum Air Flow per Zone Floor Area {m3/s-m2}",
        "    ,                        !- Cooling Minimum Air Flow {m3/s}",
        "    0,                       !- Cooling Minimum Air Flow Fraction",
        "    DesignDay,               !- Heating Design Air Flow Method",
        "    0,                       !- Heating Design Air Flow Rate {m3/s}",
        "    ,                        !- Heating Maximum Air Flow per Zone Floor Area {m3/s-m2}",
        "    ,                        !- Heating Maximum Air Flow {m3/s}",
        "    0,                       !- Heating Maximum Air Flow Fraction",
        "    SZ DSZAD NORTH_ZONE,     !- Design Specification Zone Air Distribution Object Name",
        "    No,                      !- Account for Dedicated Outdoor Air System",
        "    NeutralSupplyAir,        !- Dedicated Outdoor Air System Control Strategy",
        "    autosize,                !- Dedicated Outdoor Air Low Setpoint Temperature for Design {C}",
        "    autosize;                !- Dedicated Outdoor Air High Setpoint Temperature for Design {C}",

        "ZoneControl:Thermostat,",
        "    NORTH_ZONE Thermostat,   !- Name",
        "    NORTH_ZONE,              !- Zone or ZoneList Name",
        "    HVACTemplate-Always 4,   !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    All Zones Dual SP Control;  !- Control 1 Name",

        "ThermostatSetpoint:DualSetpoint,",
        "    All Zones Dual SP Control,  !- Name",
        "    Htg-SetP-Sch,            !- Heating Setpoint Temperature Schedule Name",
        "    Clg-SetP-Sch;            !- Cooling Setpoint Temperature Schedule Name",

        "ZoneHVAC:PackagedTerminalAirConditioner,",
        "    NORTH_ZONE PTAC,         !- Name",
        "    HVACTemplate-Always 1,   !- Availability Schedule Name",
        "    NORTH_ZONE PTAC Return,  !- Air Inlet Node Name",
        "    NORTH_ZONE PTAC Supply Inlet,  !- Air Outlet Node Name",
        "    OutdoorAir:Mixer,        !- Outdoor Air Mixer Object Type",
        "    NORTH_ZONE PTAC OA Mixing Box,  !- Outdoor Air Mixer Name",
        "    autosize,                !- Cooling Supply Air Flow Rate {m3/s}",
        "    autosize,                !- Heating Supply Air Flow Rate {m3/s}",
        "    ,                        !- No Load Supply Air Flow Rate {m3/s}",
        "    autosize,                !- Cooling Outdoor Air Flow Rate {m3/s}",
        "    autosize,                !- Heating Outdoor Air Flow Rate {m3/s}",
        "    autosize,                !- No Load Outdoor Air Flow Rate {m3/s}",
        "    Fan:OnOff,               !- Supply Air Fan Object Type",
        "    NORTH_ZONE PTAC Supply Fan,  !- Supply Air Fan Name",
        "    Coil:Heating:Electric,   !- Heating Coil Object Type",
        "    NORTH_ZONE PTAC Heating Coil,  !- Heating Coil Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Cooling Coil Object Type",
        "    NORTH_ZONE PTAC Cooling Coil,  !- Cooling Coil Name",
        "    DrawThrough,             !- Fan Placement",
        "    HVACTemplate-Always 0;   !- Supply Air Fan Operating Mode Schedule Name",

        "ZoneHVAC:EquipmentList,",
        "    NORTH_ZONE Equipment,    !- Name",
        "    SequentialLoad,          !- Load Distribution Scheme",
        "    ZoneHVAC:PackagedTerminalAirConditioner,  !- Zone Equipment 1 Object Type",
        "    NORTH_ZONE PTAC,         !- Zone Equipment 1 Name",
        "    1,                       !- Zone Equipment 1 Cooling Sequence",
        "    1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "    ,                        !- Zone Equipment 1 Sequential Cooling Fraction Schedule Name",
        "    ;                        !- Zone Equipment 1 Sequential Heating Fraction Schedule Name",

        "ZoneHVAC:EquipmentConnections,",
        "    NORTH_ZONE,              !- Zone Name",
        "    NORTH_ZONE Equipment,    !- Zone Conditioning Equipment List Name",
        "    NORTH_ZONE PTAC Supply Inlet,  !- Zone Air Inlet Node or NodeList Name",
        "    NORTH_ZONE PTAC Return,  !- Zone Air Exhaust Node or NodeList Name",
        "    NORTH_ZONE Zone Air Node,!- Zone Air Node Name",
        "    NORTH_ZONE Return Outlet;!- Zone Return Air Node or NodeList Name",

        "Fan:OnOff,",
        "    NORTH_ZONE PTAC Supply Fan,  !- Name",
        "    HVACTemplate-Always 1,   !- Availability Schedule Name",
        "    0.7,                     !- Fan Total Efficiency",
        "    75,                      !- Pressure Rise {Pa}",
        "    autosize,                !- Maximum Flow Rate {m3/s}",
        "    0.9,                     !- Motor Efficiency",
        "    1,                       !- Motor In Airstream Fraction",
        "    NORTH_ZONE PTAC Heating Coil Outlet,  !- Air Inlet Node Name",
        "    NORTH_ZONE PTAC Supply Inlet;  !- Air Outlet Node Name",

        "Coil:Cooling:DX:SingleSpeed,",
        "    NORTH_ZONE PTAC Cooling Coil,  !- Name",
        "    HVACTemplate-Always 1,   !- Availability Schedule Name",
        "    autosize,                !- Gross Rated Total Cooling Capacity {W}",
        "    autosize,                !- Gross Rated Sensible Heat Ratio",
        "    3,                       !- Gross Rated Cooling COP {W/W}",
        "    autosize,                !- Rated Air Flow Rate {m3/s}",
        "    ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    NORTH_ZONE PTAC Mixed Air Outlet,  !- Air Inlet Node Name",
        "    NORTH_ZONE PTAC Cooling Coil Outlet,  !- Air Outlet Node Name",
        "    NORTH_ZONE PTAC Cooling Coil Cap-FT,  !- Total Cooling Capacity Function of Temperature Curve Name",
        "    NORTH_ZONE PTAC Cooling Coil Cap-FF,  !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "    NORTH_ZONE PTAC Cooling Coil EIR-FT,  !- Energy Input Ratio Function of Temperature Curve Name",
        "    NORTH_ZONE PTAC Cooling Coil EIR-FF,  !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "    NORTH_ZONE PTAC Cooling Coil Coil PLF,  !- Part Load Fraction Correlation Curve Name",
        "    ,                        !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "    0,                       !- Nominal Time for Condensate Removal to Begin {s}",
        "    0,                       !- Ratio of Initial Moisture Evaporation Rate and Steady State Latent Capacity {dimensionless}",
        "    0,                       !- Maximum Cycling Rate {cycles/hr}",
        "    0,                       !- Latent Capacity Time Constant {s}",
        "    NORTH_ZONE PTAC Cooling Coil Condenser Inlet,  !- Condenser Air Inlet Node Name",
        "    AirCooled,               !- Condenser Type",
        "    0,                       !- Evaporative Condenser Effectiveness {dimensionless}",
        "    ,                        !- Evaporative Condenser Air Flow Rate {m3/s}",
        "    0,                       !- Evaporative Condenser Pump Rated Power Consumption {W}",
        "    0,                       !- Crankcase Heater Capacity {W}",
        "    10;                      !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",

        "Coil:Heating:Electric,",
        "    NORTH_ZONE PTAC Heating Coil,  !- Name",
        "    HVACTemplate-Always 1,   !- Availability Schedule Name",
        "    1,                       !- Efficiency",
        "    autosize,                !- Nominal Capacity {W}",
        "    NORTH_ZONE PTAC Cooling Coil Outlet,  !- Air Inlet Node Name",
        "    NORTH_ZONE PTAC Heating Coil Outlet;  !- Air Outlet Node Name",

        "OutdoorAir:Mixer,",
        "    NORTH_ZONE PTAC OA Mixing Box,  !- Name",
        "    NORTH_ZONE PTAC Mixed Air Outlet,  !- Mixed Air Node Name",
        "    NORTH_ZONE PTAC Outside Air Inlet,  !- Outdoor Air Stream Node Name",
        "    NORTH_ZONE PTAC Relief Air Outlet,  !- Relief Air Stream Node Name",
        "    NORTH_ZONE PTAC Return;  !- Return Air Stream Node Name",

        "OutdoorAir:Node,",
        "    NORTH_ZONE PTAC Outside Air Inlet,  !- Name",
        "    -1;                      !- Height Above Ground {m}",

        "OutdoorAir:Node,",
        "    NORTH_ZONE PTAC Cooling Coil Condenser Inlet,  !- Name",
        "    -1;                      !- Height Above Ground {m}",

        "Curve:Quadratic,",
        "    NORTH_ZONE PTAC Cooling Coil Cap-FF,  !- Name",
        "    0.8,                     !- Coefficient1 Constant",
        "    0.2,                     !- Coefficient2 x",
        "    0,                       !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "    NORTH_ZONE PTAC Cooling Coil EIR-FF,  !- Name",
        "    1.1552,                  !- Coefficient1 Constant",
        "    -0.1808,                 !- Coefficient2 x",
        "    0.0256,                  !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "Curve:Quadratic,",
        "    NORTH_ZONE PTAC Cooling Coil Coil PLF,  !- Name",
        "    0.85,                    !- Coefficient1 Constant",
        "    0.15,                    !- Coefficient2 x",
        "    0,                       !- Coefficient3 x**2",
        "    0,                       !- Minimum Value of x",
        "    1;                       !- Maximum Value of x",

        "Curve:Biquadratic,",
        "    NORTH_ZONE PTAC Cooling Coil Cap-FT,  !- Name",
        "    0.942587793,             !- Coefficient1 Constant",
        "    0.009543347,             !- Coefficient2 x",
        "    0.00068377,              !- Coefficient3 x**2",
        "    -0.011042676,            !- Coefficient4 y",
        "    0.000005249,             !- Coefficient5 y**2",
        "    -0.00000972,             !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111;                !- Maximum Value of y",

        "Curve:Biquadratic,",
        "    NORTH_ZONE PTAC Cooling Coil EIR-FT,  !- Name",
        "    0.342414409,             !- Coefficient1 Constant",
        "    0.034885008,             !- Coefficient2 x",
        "    -0.0006237,              !- Coefficient3 x**2",
        "    0.004977216,             !- Coefficient4 y",
        "    0.000437951,             !- Coefficient5 y**2",
        "    -0.000728028,            !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111;                !- Maximum Value of y",

    });

    ASSERT_TRUE(process_idf(idf_objects));
    bool ErrorsFound = false;
    state->dataGlobal->NumOfTimeStepInHour = 1;
    state->dataGlobal->MinutesPerTimeStep = 60;
    // Read objects
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetWindowGlassSpectralData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    Material::GetMaterialData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    SurfaceGeometry::GetGeometryParameters(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosBldgRotAppGonly = 1.0;
    state->dataSurfaceGeometry->SinBldgRotAppGonly = 0.0;
    SurfaceGeometry::GetSurfaceData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    ScheduleManager::ProcessScheduleInput(*state);
    state->dataScheduleMgr->Schedule(6).MinValue = 0.0;
    state->dataScheduleMgr->Schedule(6).MaxValue = 2.0;
    state->dataScheduleMgr->DaySchedule(1).TSValue = 150;
    state->dataScheduleMgr->DaySchedule(1).TSValMax = 200;
    state->dataScheduleMgr->DaySchedule(1).TSValMin = 0;
    state->dataScheduleMgr->DaySchedule(2).TSValue = 1.0;
    state->dataScheduleMgr->DaySchedule(3).TSValue = 1.0;
    state->dataScheduleMgr->Schedule(2).MinValue = 0.0;
    state->dataScheduleMgr->Schedule(2).MaxValue = 1.0;
    state->dataScheduleMgr->Schedule(3).MinValue = 0.0;
    state->dataScheduleMgr->Schedule(3).MaxValue = 1.0;

    InternalHeatGains::GetInternalHeatGainsInput(*state);
    HeatBalanceAirManager::GetRoomAirModelParameters(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    // Read AirflowNetwork inputs
    state->afn->get_input();

    EXPECT_EQ(state->afn->IntraZoneNodeData(1).AFNZoneNum, 1);
    EXPECT_EQ(state->afn->IntraZoneNodeData(1).ZoneNum, 3);
}

TEST_F(EnergyPlusFixture, AirflowNetwork_TestReferenceConditionsLeftBlank)
{
    // unit test for the object of ReferenceCrackConditions when reference conditions was left blank
    bool ErrorsFound(false);

    std::string const idf_objects = delimited_string({
        "SurfaceConvectionAlgorithm:Inside,TARP;",
        "SurfaceConvectionAlgorithm:Outside,DOE-2;",
        "HeatBalanceAlgorithm,ConductionTransferFunction;",
        "ZoneAirHeatBalanceAlgorithm,",
        "AnalyticalSolution;      !- Algorithm",
        "Schedule:Constant,OnSch,,1.0;",
        "Schedule:Constant,Temp21,,21.0;",
        "Zone,",
        "    WEST_ZONE,               !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",
        "BuildingSurface:Detailed,",
        "    Surface_1,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION,               !- Construction Name",
        "    WEST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.096000,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.096000,0,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",
        "BuildingSurface:Detailed,",
        "    Surface_2,           !- Name",
        "    Wall,                    !- Surface Type",
        "    PARTITION,             !- Construction Name",
        "    WEST_ZONE,               !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    6.096000,6.096000,3.048000,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.096000,6.096000,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,6.096000,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,6.096000,3.048000;  !- X,Y,Z ==> Vertex 4 {m}",
        "Construction,",
        "    PARTITION,             !- Name",
        "    GYP BOARD;  !- Outside Layer",
        "Material,",
        "    GYP BOARD,  !- Name",
        "    Smooth,                  !- Roughness",
        "    1.9050000E-02,           !- Thickness {m}",
        "    0.7264224,               !- Conductivity {W/m-K}",
        "    1601.846,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",
        "AirflowNetwork:SimulationControl,",
        "   NaturalVentilation, !- Name",
        "   MultizoneWithoutDistribution, !- AirflowNetwork Control",
        "   SurfaceAverageCalculation, !- Wind Pressure Coefficient Type",
        "   , !- Height Selection for Local Wind Pressure Calculation",
        "   LOWRISE, !- Building Type",
        "   1000, !- Maximum Number of Iterations{ dimensionless }",
        "   LinearInitializationMethod, !- Initialization Type",
        "   0.0001, !- Relative Airflow Convergence Tolerance{ dimensionless }",
        "   0.0001, !- Absolute Airflow Convergence Tolerance{ kg / s }",
        "   -0.5, !- Convergence Acceleration Limit{ dimensionless }",
        "   90, !- Azimuth Angle of Long Axis of Building{ deg }",
        "   0.36; !- Ratio of Building Width Along Short Axis to Width Along Long Axis",
        "AirflowNetwork:MultiZone:Zone,",
        "   WEST_ZONE, !- Zone Name",
        "   Temperature, !- Ventilation Control Mode",
        "   Temp21, !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "   1, !- Minimum Venting Open Factor{ dimensionless }",
        "   , !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor{ deltaC }",
        "   100, !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor{ deltaC }",
        "   , !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor{ deltaJ / kg }",
        "   300000, !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor{ deltaJ / kg }",
        "   OnSch, !- Venting Availability Schedule Name",
        "   Standard;  !- Single Sided Wind Pressure Coefficient Algorithm",
        "AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "   ReferenceCrackConditions1, !- Name",
        "   20, !- Reference Temperature{ C }",
        "   , !- Reference Barometric Pressure{ Pa }",
        "   ; !- Reference Humidity Ratio{ kgWater / kgDryAir }",
        "AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "   ReferenceCrackConditions2, !- Name",
        "   30, !- Reference Temperature{ C }",
        "   50000, !- Reference Barometric Pressure{ Pa }",
        "   0.002; !- Reference Humidity Ratio{ kgWater / kgDryAir }",
        "AirflowNetwork:MultiZone:Surface,",
        "   Surface_1, !- Surface Name",
        "   CR-1, !- Leakage Component Name",
        "   , !- External Node Name",
        "   1; !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "AirflowNetwork:MultiZone:Surface,",
        "   Surface_2, !- Surface Name",
        "   CR-2, !- Leakage Component Name",
        "   , !- External Node Name",
        "   1; !- Window / Door Opening Factor, or Crack Factor{ dimensionless }",
        "AirflowNetwork:MultiZone:Surface:Crack,",
        "   CR-1, !- Name",
        "   0.01, !- Air Mass Flow Coefficient at Reference Conditions{ kg / s }",
        "   0.667, !- Air Mass Flow Exponent{ dimensionless }",
        "   ReferenceCrackConditions1; !- Reference Crack Conditions",
        "AirflowNetwork:MultiZone:Surface:Crack,",
        "   CR-2, !- Name",
        "   0.01, !- Air Mass Flow Coefficient at Reference Conditions{ kg / s }",
        "   0.667, !- Air Mass Flow Exponent{ dimensionless }",
        "   ReferenceCrackConditions2; !- Reference Crack Conditions",

    });

    ASSERT_TRUE(process_idf(idf_objects));

    ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    ErrorsFound = false;
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    ErrorsFound = false;
    Material::GetMaterialData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    ErrorsFound = false;
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    ErrorsFound = false;
    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = std::cos(-state->dataHeatBal->Zone(1).RelNorth * Constant::DegToRadians);
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = std::sin(-state->dataHeatBal->Zone(1).RelNorth * Constant::DegToRadians);
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;
    SurfaceGeometry::GetSurfaceData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    state->afn->get_input();
    // check correct values for reference conditions of crack surface when reference conditions were left blank.
    Real64 refP1 = 101325.0;
    Real64 refT1 = 20.0;
    Real64 refW1 = 0.0;
    Real64 expected_density1 = state->afn->properties.density(refP1, refT1, refW1);
    Real64 expected_viscosity1 = state->afn->properties.dynamic_viscosity(refT1);
    EXPECT_EQ(expected_density1, state->afn->MultizoneSurfaceCrackData(1).reference_density);
    EXPECT_EQ(expected_viscosity1, state->afn->MultizoneSurfaceCrackData(1).reference_viscosity);
    Real64 refP2 = 50000.0;
    Real64 refT2 = 30.0;
    Real64 refW2 = 0.002;
    Real64 expected_density2 = state->afn->properties.density(refP2, refT2, refW2);
    Real64 expected_viscosity2 = state->afn->properties.dynamic_viscosity(refT2);
    EXPECT_EQ(expected_density2, state->afn->MultizoneSurfaceCrackData(2).reference_density);
    EXPECT_EQ(expected_viscosity2, state->afn->MultizoneSurfaceCrackData(2).reference_viscosity);
}

TEST_F(EnergyPlusFixture, AirflowNetwork_DuctSizingTest)
{

    // Unit test for a new feature of AFN Fan Model
    int i;

    std::string const idf_objects = delimited_string({
        "  Building,",
        "    House with AirflowNetwork simulation,  !- Name",
        "    0,                       !- North Axis {deg}",
        "    Suburbs,                 !- Terrain",
        "    0.001,                   !- Loads Convergence Tolerance Value",
        "    0.0050000,               !- Temperature Convergence Tolerance Value {deltaC}",
        "    FullInteriorAndExterior, !- Solar Distribution",
        "    25,                      !- Maximum Number of Warmup Days",
        "    6;                       !- Minimum Number of Warmup Days",

        "  Timestep,6;",

        "  SurfaceConvectionAlgorithm:Inside,TARP;",

        "  SurfaceConvectionAlgorithm:Outside,DOE-2;",

        "  HeatBalanceAlgorithm,ConductionTransferFunction;",

        "  SimulationControl,",
        "    No,                      !- Do Zone Sizing Calculation",
        "    No,                      !- Do System Sizing Calculation",
        "    No,                      !- Do Plant Sizing Calculation",
        "    Yes,                     !- Run Simulation for Sizing Periods",
        "    No;                      !- Run Simulation for Weather File Run Periods",

        "  RunPeriod,",
        "    RunPeriod 1,             !- Name",
        "    1,                       !- Begin Month",
        "    14,                      !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    1,                       !- End Month",
        "    14,                      !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    Yes;                     !- Use Weather File Snow Indicators",

        "  RunPeriod,",
        "    RunPeriod 2,             !- Name",
        "    7,                       !- Begin Month",
        "    7,                       !- Begin Day of Month",
        "    ,                        !- Begin Year",
        "    7,                       !- End Month",
        "    7,                       !- End Day of Month",
        "    ,                        !- End Year",
        "    Tuesday,                 !- Day of Week for Start Day",
        "    Yes,                     !- Use Weather File Holidays and Special Days",
        "    Yes,                     !- Use Weather File Daylight Saving Period",
        "    No,                      !- Apply Weekend Holiday Rule",
        "    Yes,                     !- Use Weather File Rain Indicators",
        "    No;                      !- Use Weather File Snow Indicators",

        "  Site:Location,",
        "    CHICAGO_IL_USA TMY2-94846,  !- Name",
        "    41.78,                   !- Latitude {deg}",
        "    -87.75,                  !- Longitude {deg}",
        "    -6.00,                   !- Time Zone {hr}",
        "    190.00;                  !- Elevation {m}",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Heating 99% Design Conditions DB,  !- Name",
        "    1,                       !- Month",
        "    21,                      !- Day of Month",
        "    WinterDesignDay,         !- Day Type",
        "    -17.3,                   !- Maximum Dry-Bulb Temperature {C}",
        "    0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    -17.3,                   !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    4.9,                     !- Wind Speed {m/s}",
        "    270,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    0.0;                     !- Sky Clearness",

        "  SizingPeriod:DesignDay,",
        "    CHICAGO_IL_USA Annual Cooling 1% Design Conditions DB/MCWB,  !- Name",
        "    7,                       !- Month",
        "    21,                      !- Day of Month",
        "    SummerDesignDay,         !- Day Type",
        "    31.5,                    !- Maximum Dry-Bulb Temperature {C}",
        "    10.7,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Type",
        "    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
        "    Wetbulb,                 !- Humidity Condition Type",
        "    23.0,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
        "    ,                        !- Humidity Condition Day Schedule Name",
        "    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
        "    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
        "    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
        "    99063.,                  !- Barometric Pressure {Pa}",
        "    5.3,                     !- Wind Speed {m/s}",
        "    230,                     !- Wind Direction {deg}",
        "    No,                      !- Rain Indicator",
        "    No,                      !- Snow Indicator",
        "    No,                      !- Daylight Saving Time Indicator",
        "    ASHRAEClearSky,          !- Solar Model Indicator",
        "    ,                        !- Beam Solar Day Schedule Name",
        "    ,                        !- Diffuse Solar Day Schedule Name",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
        "    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
        "    1.0;                     !- Sky Clearness",

        "  Site:GroundTemperature:BuildingSurface,20.03,20.03,20.13,20.30,20.43,20.52,20.62,20.77,20.78,20.55,20.44,20.20;",

        "  Material,",
        "    A1 - 1 IN STUCCO,        !- Name",
        "    Smooth,                  !- Roughness",
        "    2.5389841E-02,           !- Thickness {m}",
        "    0.6918309,               !- Conductivity {W/m-K}",
        "    1858.142,                !- Density {kg/m3}",
        "    836.8000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.9200000,               !- Solar Absorptance",
        "    0.9200000;               !- Visible Absorptance",

        "  Material,",
        "    CB11,                    !- Name",
        "    MediumRough,             !- Roughness",
        "    0.2032000,               !- Thickness {m}",
        "    1.048000,                !- Conductivity {W/m-K}",
        "    1105.000,                !- Density {kg/m3}",
        "    837.0000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.2000000,               !- Solar Absorptance",
        "    0.2000000;               !- Visible Absorptance",

        "  Material,",
        "    GP01,                    !- Name",
        "    MediumSmooth,            !- Roughness",
        "    1.2700000E-02,           !- Thickness {m}",
        "    0.1600000,               !- Conductivity {W/m-K}",
        "    801.0000,                !- Density {kg/m3}",
        "    837.0000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "  Material,",
        "    IN02,                    !- Name",
        "    Rough,                   !- Roughness",
        "    9.0099998E-02,           !- Thickness {m}",
        "    4.3000001E-02,           !- Conductivity {W/m-K}",
        "    10.00000,                !- Density {kg/m3}",
        "    837.0000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "  Material,",
        "    IN05,                    !- Name",
        "    Rough,                   !- Roughness",
        "    0.2458000,               !- Thickness {m}",
        "    4.3000001E-02,           !- Conductivity {W/m-K}",
        "    10.00000,                !- Density {kg/m3}",
        "    837.0000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "  Material,",
        "    PW03,                    !- Name",
        "    MediumSmooth,            !- Roughness",
        "    1.2700000E-02,           !- Thickness {m}",
        "    0.1150000,               !- Conductivity {W/m-K}",
        "    545.0000,                !- Density {kg/m3}",
        "    1213.000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7800000,               !- Solar Absorptance",
        "    0.7800000;               !- Visible Absorptance",

        "  Material,",
        "    CC03,                    !- Name",
        "    MediumRough,             !- Roughness",
        "    0.1016000,               !- Thickness {m}",
        "    1.310000,                !- Conductivity {W/m-K}",
        "    2243.000,                !- Density {kg/m3}",
        "    837.0000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.6500000,               !- Solar Absorptance",
        "    0.6500000;               !- Visible Absorptance",

        "  Material,",
        "    HF-A3,                   !- Name",
        "    Smooth,                  !- Roughness",
        "    1.5000000E-03,           !- Thickness {m}",
        "    44.96960,                !- Conductivity {W/m-K}",
        "    7689.000,                !- Density {kg/m3}",
        "    418.0000,                !- Specific Heat {J/kg-K}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.2000000,               !- Solar Absorptance",
        "    0.2000000;               !- Visible Absorptance",

        "  Material:NoMass,",
        "    AR02,                    !- Name",
        "    VeryRough,               !- Roughness",
        "    7.8000002E-02,           !- Thermal Resistance {m2-K/W}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7000000,               !- Solar Absorptance",
        "    0.7000000;               !- Visible Absorptance",

        "  Material:NoMass,",
        "    CP02,                    !- Name",
        "    Rough,                   !- Roughness",
        "    0.2170000,               !- Thermal Resistance {m2-K/W}",
        "    0.9000000,               !- Thermal Absorptance",
        "    0.7500000,               !- Solar Absorptance",
        "    0.7500000;               !- Visible Absorptance",

        "  WindowMaterial:Glazing,",
        "    CLEAR 3MM,               !- Name",
        "    SpectralAverage,         !- Optical Data Type",
        "    ,                        !- Window Glass Spectral Data Set Name",
        "    0.003,                   !- Thickness {m}",
        "    0.837,                   !- Solar Transmittance at Normal Incidence",
        "    0.075,                   !- Front Side Solar Reflectance at Normal Incidence",
        "    0.075,                   !- Back Side Solar Reflectance at Normal Incidence",
        "    0.898,                   !- Visible Transmittance at Normal Incidence",
        "    0.081,                   !- Front Side Visible Reflectance at Normal Incidence",
        "    0.081,                   !- Back Side Visible Reflectance at Normal Incidence",
        "    0.0,                     !- Infrared Transmittance at Normal Incidence",
        "    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
        "    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
        "    0.9;                     !- Conductivity {W/m-K}",

        "  WindowMaterial:Gas,",
        "    AIR 6MM,                 !- Name",
        "    AIR,                     !- Gas Type",
        "    0.006;                   !- Thickness {m}",

        "  Construction,",
        "    EXTWALL:LIVING,          !- Name",
        "    A1 - 1 IN STUCCO,        !- Outside Layer",
        "    CB11,                    !- Layer 2",
        "    GP01;                    !- Layer 3",

        "  Construction,",
        "    INTERIORWall,            !- Name",
        "    GP01,                    !- Outside Layer",
        "    IN02,                    !- Layer 2",
        "    GP01;                    !- Layer 3",

        "  Construction,",
        "    FLOOR:GARAGE,            !- Name",
        "    CC03;                    !- Outside Layer",

        "  Construction,",
        "    FLOOR:LIVING,            !- Name",
        "    CC03,                    !- Outside Layer",
        "    CP02;                    !- Layer 2",

        "  Construction,",
        "    ROOF,                    !- Name",
        "    AR02,                    !- Outside Layer",
        "    PW03;                    !- Layer 2",

        "  Construction,",
        "    EXTWALL:GARAGE,          !- Name",
        "    A1 - 1 IN STUCCO,        !- Outside Layer",
        "    CB11;                    !- Layer 2",

        "  Construction,",
        "    CEILING:LIVING,          !- Name",
        "    IN05,                    !- Outside Layer",
        "    GP01;                    !- Layer 2",

        "  Construction,",
        "    reverseCEILING:LIVING,   !- Name",
        "    GP01,                    !- Outside Layer",
        "    IN05;                    !- Layer 2",

        "  Construction,",
        "    GABLE,                   !- Name",
        "    PW03;                    !- Outside Layer",

        "  Construction,",
        "    Dbl Clr 3mm/6mm Air,     !- Name",
        "    CLEAR 3MM,               !- Outside Layer",
        "    AIR 6MM,                 !- Layer 2",
        "    CLEAR 3MM;               !- Layer 3",

        "  Construction,",
        "    Garage:SteelDoor,        !- Name",
        "    HF-A3;                   !- Outside Layer",

        "  Construction,",
        "    CEILING:Garage,          !- Name",
        "    GP01;                    !- Outside Layer",

        "  Zone,",
        "    LIVING ZONE,             !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    GARAGE ZONE,             !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  Zone,",
        "    ATTIC ZONE,              !- Name",
        "    0,                       !- Direction of Relative North {deg}",
        "    0,                       !- X Origin {m}",
        "    0,                       !- Y Origin {m}",
        "    0,                       !- Z Origin {m}",
        "    1,                       !- Type",
        "    1,                       !- Multiplier",
        "    autocalculate,           !- Ceiling Height {m}",
        "    autocalculate;           !- Volume {m3}",

        "  GlobalGeometryRules,",
        "    UpperLeftCorner,         !- Starting Vertex Position",
        "    CounterClockWise,        !- Vertex Entry Direction",
        "    World;                   !- Coordinate System",

        "  BuildingSurface:Detailed,",
        "    Living:North,            !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL:LIVING,          !- Construction Name",
        "    LIVING ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    10.323,10.778,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.323,10.778,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,10.778,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,10.778,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Living:East,             !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL:LIVING,          !- Construction Name",
        "    LIVING ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    17.242,0,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,10.778,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,10.778,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Living:South,            !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL:LIVING,          !- Construction Name",
        "    LIVING ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,2.4383,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,0,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Living:West,             !- Name",
        "    Wall,                    !- Surface Type",
        "    EXTWALL:LIVING,          !- Construction Name",
        "    LIVING ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,10.778,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,10.778,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,0,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,0,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Garage:Interior,         !- Name",
        "    WALL,                    !- Surface Type",
        "    INTERIORWall,            !- Construction Name",
        "    GARAGE ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Living:Interior,         !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    10.323,10.778,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.323,10.778,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,10.778,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,10.778,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Living:Interior,         !- Name",
        "    WALL,                    !- Surface Type",
        "    INTERIORWall,            !- Construction Name",
        "    LIVING ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Garage:Interior,         !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    17.242,10.778,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,10.778,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    10.323,10.778,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    10.323,10.778,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Living:Floor,            !- Name",
        "    FLOOR,                   !- Surface Type",
        "    FLOOR:LIVING,            !- Construction Name",
        "    LIVING ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Living:Floor,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,10.778,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,10.778,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,0,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Living:Ceiling,          !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:LIVING,          !- Construction Name",
        "    LIVING ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Attic:LivingFloor,       !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,10.778,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,0,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,0,2.4384,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,10.778,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Attic:LivingFloor,       !- Name",
        "    FLOOR,                   !- Surface Type",
        "    reverseCEILING:LIVING,   !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Living:Ceiling,          !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0,0,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,10.778,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,10.778,2.4384,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,0,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    NorthRoof1,              !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.9,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    13.782,5.389,4.6838,  !- X,Y,Z ==> Vertex 1 {m}",
        "    13.782,7.3172,3.8804,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,7.3172,3.8804,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,5.389,4.6838;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    SouthRoof,               !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5000000,               !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    0.000000,5.389000,4.683800,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0.000000,0.000000,2.438400,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.24200,0.000000,2.438400,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.24200,5.389000,4.683800;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    NorthRoof2,              !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.9,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    13.782,7.3172,3.8804,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.332,10.778,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0.0,10.778,2.4384,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0.0,7.3172,3.8804;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    NorthRoof3,              !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.9,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    17.242,5.389,4.6838,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,7.3172,3.8804,  !- X,Y,Z ==> Vertex 2 {m}",
        "    13.782,7.3172,3.8804,  !- X,Y,Z ==> Vertex 3 {m}",
        "    13.782,5.389,4.6838;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    NorthRoof4,              !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.9,                     !- View Factor to Ground",
        "    3,                       !- Number of Vertices",
        "    17.242,7.3172,3.8804,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,10.778,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    13.782,7.3172,3.8804;  !- X,Y,Z ==> Vertex 3 {m}",

        "  BuildingSurface:Detailed,",
        "    EastGable,               !- Name",
        "    WALL,                    !- Surface Type",
        "    GABLE,                   !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    3,                       !- Number of Vertices",
        "    17.242,5.389,4.6838,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,0.0,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,10.778,2.4384;  !- X,Y,Z ==> Vertex 3 {m}",

        "  BuildingSurface:Detailed,",
        "    WestGable,               !- Name",
        "    WALL,                    !- Surface Type",
        "    GABLE,                   !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    3,                       !- Number of Vertices",
        "    0.0,5.389,4.6838,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0.0,10.778,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0.0,0.0,2.4384;  !- X,Y,Z ==> Vertex 3 {m}",

        "  BuildingSurface:Detailed,",
        "    EastRoof,                !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.9,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    13.782,16.876,3.8804,  !- X,Y,Z ==> Vertex 1 {m}",
        "    13.782,7.3172,3.8804,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,10.778,2.4384,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,16.876,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    WestRoof,                !- Name",
        "    ROOF,                    !- Surface Type",
        "    ROOF,                    !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.9,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    10.323,16.876,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.323,10.778,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    13.782,7.3172,3.8804,  !- X,Y,Z ==> Vertex 3 {m}",
        "    13.782,16.876,3.8804;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Attic:NorthGable,        !- Name",
        "    WALL,                    !- Surface Type",
        "    GABLE,                   !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    3,                       !- Number of Vertices",
        "    13.782,16.876,3.8804,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,16.876,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    10.323,16.876,2.4384;  !- X,Y,Z ==> Vertex 3 {m}",

        "  BuildingSurface:Detailed,",
        "    Garage:EastWall,         !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL:GARAGE,          !- Construction Name",
        "    GARAGE ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    17.242,10.778,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,10.778,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,16.876,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,16.876,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Garage:WestWall,         !- Name",
        "    WALL,                    !- Surface Type",
        "    EXTWALL:GARAGE,          !- Construction Name",
        "    GARAGE ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    10.323,16.876,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.323,16.876,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    10.323,10.778,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    10.323,10.778,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Garage:FrontDoor,        !- Name",
        "    WALL,                    !- Surface Type",
        "    Garage:SteelDoor,        !- Construction Name",
        "    GARAGE ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Outdoors,                !- Outside Boundary Condition",
        "    ,                        !- Outside Boundary Condition Object",
        "    SunExposed,              !- Sun Exposure",
        "    WindExposed,             !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    17.242,16.876,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,16.876,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    10.323,16.876,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    10.323,16.876,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Attic:GarageFloor,       !- Name",
        "    FLOOR,                   !- Surface Type",
        "    CEILING:Garage,          !- Construction Name",
        "    ATTIC ZONE,              !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Garage:Ceiling,          !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    10.323,10.778,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.323,16.876,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,16.876,2.4384,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,10.778,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Garage:Ceiling,          !- Name",
        "    CEILING,                 !- Surface Type",
        "    CEILING:Garage,          !- Construction Name",
        "    GARAGE ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Attic:GarageFloor,       !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0.5,                     !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    10.323,16.876,2.4384,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.323,10.778,2.4384,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,10.778,2.4384,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,16.876,2.4384;  !- X,Y,Z ==> Vertex 4 {m}",

        "  BuildingSurface:Detailed,",
        "    Garage:Floor,            !- Name",
        "    FLOOR,                   !- Surface Type",
        "    FLOOR:GARAGE,            !- Construction Name",
        "    GARAGE ZONE,             !- Zone Name",
        "    ,                        !- Space Name",
        "    Surface,                 !- Outside Boundary Condition",
        "    Garage:Floor,            !- Outside Boundary Condition Object",
        "    NoSun,                   !- Sun Exposure",
        "    NoWind,                  !- Wind Exposure",
        "    0,                       !- View Factor to Ground",
        "    4,                       !- Number of Vertices",
        "    10.323,10.778,0,  !- X,Y,Z ==> Vertex 1 {m}",
        "    10.323,16.876,0,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,16.876,0,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,10.778,0;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    NorthWindow,             !- Name",
        "    Window,                  !- Surface Type",
        "    Dbl Clr 3mm/6mm Air,     !- Construction Name",
        "    Living:North,            !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    6.572,10.778,2.1336,  !- X,Y,Z ==> Vertex 1 {m}",
        "    6.572,10.778,0.6096,  !- X,Y,Z ==> Vertex 2 {m}",
        "    2,10.778,0.6096,  !- X,Y,Z ==> Vertex 3 {m}",
        "    2,10.778,2.1336;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    EastWindow,              !- Name",
        "    Window,                  !- Surface Type",
        "    Dbl Clr 3mm/6mm Air,     !- Construction Name",
        "    Living:East,             !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    17.242,2,2.1336,  !- X,Y,Z ==> Vertex 1 {m}",
        "    17.242,2,0.6096,  !- X,Y,Z ==> Vertex 2 {m}",
        "    17.242,6.572,0.6096,  !- X,Y,Z ==> Vertex 3 {m}",
        "    17.242,6.572,2.1336;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    SouthWindow,             !- Name",
        "    Window,                  !- Surface Type",
        "    Dbl Clr 3mm/6mm Air,     !- Construction Name",
        "    Living:South,            !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    2,0,2.1336,  !- X,Y,Z ==> Vertex 1 {m}",
        "    2,0,0.6096,  !- X,Y,Z ==> Vertex 2 {m}",
        "    6.572,0,0.6096,  !- X,Y,Z ==> Vertex 3 {m}",
        "    6.572,0,2.1336;  !- X,Y,Z ==> Vertex 4 {m}",

        "  FenestrationSurface:Detailed,",
        "    WestWindow,              !- Name",
        "    Window,                  !- Surface Type",
        "    Dbl Clr 3mm/6mm Air,     !- Construction Name",
        "    Living:West,             !- Building Surface Name",
        "    ,                        !- Outside Boundary Condition Object",
        "    0.5000000,               !- View Factor to Ground",
        "    ,                        !- Frame and Divider Name",
        "    1.0,                     !- Multiplier",
        "    4,                       !- Number of Vertices",
        "    0,6.572,2.1336,  !- X,Y,Z ==> Vertex 1 {m}",
        "    0,6.572,0.6096,  !- X,Y,Z ==> Vertex 2 {m}",
        "    0,2,0.6096,  !- X,Y,Z ==> Vertex 3 {m}",
        "    0,2,2.1336;  !- X,Y,Z ==> Vertex 4 {m}",

        "  ScheduleTypeLimits,",
        "    Any Number;              !- Name",

        "  ScheduleTypeLimits,",
        "    Fraction,                !- Name",
        "    0.0,                     !- Lower Limit Value",
        "    1.0,                     !- Upper Limit Value",
        "    CONTINUOUS;              !- Numeric Type",

        "  ScheduleTypeLimits,",
        "    Temperature,             !- Name",
        "    -60,                     !- Lower Limit Value",
        "    200,                     !- Upper Limit Value",
        "    CONTINUOUS,              !- Numeric Type",
        "    Temperature;             !- Unit Type",

        "  ScheduleTypeLimits,",
        "    Control Type,            !- Name",
        "    0,                       !- Lower Limit Value",
        "    4,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "  ScheduleTypeLimits,",
        "    On/Off,                  !- Name",
        "    0,                       !- Lower Limit Value",
        "    1,                       !- Upper Limit Value",
        "    DISCRETE;                !- Numeric Type",

        "  Schedule:Compact,",
        "    WindowVentSched,         !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 3/31,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,25.55,      !- Field 3",
        "    Through: 9/30,           !- Field 5",
        "    For: AllDays,            !- Field 6",
        "    Until: 24:00,21.11,      !- Field 7",
        "    Through: 12/31,          !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until: 24:00,25.55;      !- Field 11",

        "  Schedule:Compact,",
        "    Activity Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,131.8;      !- Field 3",

        "  Schedule:Compact,",
        "    Work Eff Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.0;        !- Field 3",

        "  Schedule:Compact,",
        "    Clothing Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,1.0;        !- Field 3",

        "  Schedule:Compact,",
        "    Air Velo Sch,            !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,0.137;      !- Field 3",

        "  Schedule:Compact,",
        "    HOUSE OCCUPANCY,         !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,1.0,         !- Field 3",
        "    Until: 7:00,0.10,        !- Field 5",
        "    Until: 8:00,0.50,        !- Field 7",
        "    Until: 12:00,1.00,       !- Field 9",
        "    Until: 13:00,0.50,       !- Field 11",
        "    Until: 16:00,1.00,       !- Field 13",
        "    Until: 17:00,0.50,       !- Field 15",
        "    Until: 18:00,0.10,       !- Field 17",
        "    Until: 24:00,1.0,        !- Field 19",
        "    For: AllOtherDays,       !- Field 21",
        "    Until: 24:00,0.0;        !- Field 22",

        "  Schedule:Compact,",
        "    INTERMITTENT,            !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 8:00,0.0,         !- Field 3",
        "    Until: 18:00,1.00,       !- Field 5",
        "    Until: 24:00,0.0,        !- Field 7",
        "    For: AllOtherDays,       !- Field 9",
        "    Until: 24:00,0.0;        !- Field 10",

        "  Schedule:Compact,",
        "    HOUSE LIGHTING,          !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: WeekDays,           !- Field 2",
        "    Until: 6:00,0.05,        !- Field 3",
        "    Until: 7:00,0.20,        !- Field 5",
        "    Until: 17:00,1.00,       !- Field 7",
        "    Until: 18:00,0.50,       !- Field 9",
        "    Until: 24:00,0.05,       !- Field 11",
        "    For: AllOtherDays,       !- Field 13",
        "    Until: 24:00,0.05;       !- Field 14",

        "  Schedule:Compact,",
        "    ReportSch,               !- Name",
        "    on/off,                  !- Schedule Type Limits Name",
        "    Through: 1/20,           !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until:  24:00,0.0,       !- Field 3",
        "    Through: 1/21,           !- Field 5",
        "    For: AllDays,            !- Field 6",
        "    Until:  24:00,1.0,       !- Field 7",
        "    Through: 7/20,           !- Field 9",
        "    For: AllDays,            !- Field 10",
        "    Until:  24:00,0.0,       !- Field 11",
        "    Through: 7/21,           !- Field 13",
        "    For: AllDays,            !- Field 14",
        "    Until:  24:00,1.0,       !- Field 15",
        "    Through: 12/31,          !- Field 17",
        "    For: AllDays,            !- Field 18",
        "    Until:  24:00,0.0;       !- Field 19",

        "  Schedule:Compact,",
        "    HVACAvailSched,          !- Name",
        "    Fraction,                !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,1.0;        !- Field 3",

        "  Schedule:Compact,",
        "    Dual Heating Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,22.0;       !- Field 3",

        "  Schedule:Compact,",
        "    Dual Cooling Setpoints,  !- Name",
        "    Temperature,             !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,26.6;       !- Field 3",

        "  Schedule:Compact,",
        "    Dual Zone Control Type Sched,  !- Name",
        "    Control Type,            !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,4;          !- Field 3",

        "  Schedule:Compact,",
        "    CyclingFanSchedule,      !- Name",
        "    Any Number,              !- Schedule Type Limits Name",
        "    Through: 12/31,          !- Field 1",
        "    For: AllDays,            !- Field 2",
        "    Until: 24:00,1.0;        !- Field 3",

        "  People,",
        "    LIVING ZONE People,      !- Name",
        "    LIVING ZONE,             !- Zone or ZoneList Name",
        "    HOUSE OCCUPANCY,         !- Number of People Schedule Name",
        "    people,                  !- Number of People Calculation Method",
        "    3.000000,                !- Number of People",
        "    ,                        !- People per Zone Floor Area {person/m2}",
        "    ,                        !- Zone Floor Area per Person {m2/person}",
        "    0.3000000,               !- Fraction Radiant",
        "    ,                        !- Sensible Heat Fraction",
        "    Activity Sch,            !- Activity Level Schedule Name",
        "    3.82E-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}",
        "    ,                        !- Enable ASHRAE 55 Comfort Warnings",
        "    zoneaveraged,            !- Mean Radiant Temperature Calculation Type",
        "    ,                        !- Surface Name/Angle Factor List Name",
        "    Work Eff Sch,            !- Work Efficiency Schedule Name",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Schedule Name",
        "    Clothing Sch,            !- Clothing Insulation Schedule Name",
        "    Air Velo Sch,            !- Air Velocity Schedule Name",
        "    FANGER;                  !- Thermal Comfort Model 1 Type",

        "  Lights,",
        "    LIVING ZONE Lights,      !- Name",
        "    LIVING ZONE,             !- Zone or ZoneList Name",
        "    HOUSE LIGHTING,          !- Schedule Name",
        "    LightingLevel,           !- Design Level Calculation Method",
        "    1000,                    !- Lighting Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Return Air Fraction",
        "    0.2000000,               !- Fraction Radiant",
        "    0.2000000,               !- Fraction Visible",
        "    0,                       !- Fraction Replaceable",
        "    GeneralLights;           !- End-Use Subcategory",

        "  ElectricEquipment,",
        "    LIVING ZONE ElecEq,      !- Name",
        "    LIVING ZONE,             !- Zone or ZoneList Name",
        "    INTERMITTENT,            !- Schedule Name",
        "    EquipmentLevel,          !- Design Level Calculation Method",
        "    500,                     !- Design Level {W}",
        "    ,                        !- Watts per Zone Floor Area {W/m2}",
        "    ,                        !- Watts per Person {W/person}",
        "    0,                       !- Fraction Latent",
        "    0.3000000,               !- Fraction Radiant",
        "    0;                       !- Fraction Lost",

        "  Curve:Biquadratic,",
        "    WindACCoolCapFT,         !- Name",
        "    0.942587793,             !- Coefficient1 Constant",
        "    0.009543347,             !- Coefficient2 x",
        "    0.000683770,             !- Coefficient3 x**2",
        "    -0.011042676,            !- Coefficient4 y",
        "    0.000005249,             !- Coefficient5 y**2",
        "    -0.000009720,            !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111,                !- Maximum Value of y",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Temperature,             !- Input Unit Type for Y",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Biquadratic,",
        "    WindACEIRFT,             !- Name",
        "    0.342414409,             !- Coefficient1 Constant",
        "    0.034885008,             !- Coefficient2 x",
        "    -0.000623700,            !- Coefficient3 x**2",
        "    0.004977216,             !- Coefficient4 y",
        "    0.000437951,             !- Coefficient5 y**2",
        "    -0.000728028,            !- Coefficient6 x*y",
        "    12.77778,                !- Minimum Value of x",
        "    23.88889,                !- Maximum Value of x",
        "    18.0,                    !- Minimum Value of y",
        "    46.11111,                !- Maximum Value of y",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Temperature,             !- Input Unit Type for Y",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Quadratic,",
        "    HPACCOOLPLFFPLR,         !- Name",
        "    0.85,                    !- Coefficient1 Constant",
        "    0.15,                    !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.0,                     !- Minimum Value of x",
        "    1.0;                     !- Maximum Value of x",

        "  Curve:Cubic,",
        "    HPACHeatCapFT,           !- Name",
        "    0.758746,                !- Coefficient1 Constant",
        "    0.027626,                !- Coefficient2 x",
        "    0.000148716,             !- Coefficient3 x**2",
        "    0.0000034992,            !- Coefficient4 x**3",
        "    -20.0,                   !- Minimum Value of x",
        "    20.0,                    !- Maximum Value of x",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Cubic,",
        "    HPACHeatCapFFF,          !- Name",
        "    0.84,                    !- Coefficient1 Constant",
        "    0.16,                    !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.0,                     !- Coefficient4 x**3",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "  Curve:Cubic,",
        "    HPACHeatEIRFT,           !- Name",
        "    1.19248,                 !- Coefficient1 Constant",
        "    -0.0300438,              !- Coefficient2 x",
        "    0.00103745,              !- Coefficient3 x**2",
        "    -0.000023328,            !- Coefficient4 x**3",
        "    -20.0,                   !- Minimum Value of x",
        "    20.0,                    !- Maximum Value of x",
        "    ,                        !- Minimum Curve Output",
        "    ,                        !- Maximum Curve Output",
        "    Temperature,             !- Input Unit Type for X",
        "    Dimensionless;           !- Output Unit Type",

        "  Curve:Quadratic,",
        "    HPACHeatEIRFFF,          !- Name",
        "    1.3824,                  !- Coefficient1 Constant",
        "    -0.4336,                 !- Coefficient2 x",
        "    0.0512,                  !- Coefficient3 x**2",
        "    0.0,                     !- Minimum Value of x",
        "    1.0;                     !- Maximum Value of x",

        "  Curve:Quadratic,",
        "    WindACCoolCapFFF,        !- Name",
        "    0.8,                     !- Coefficient1 Constant",
        "    0.2,                     !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "  Curve:Quadratic,",
        "    WindACEIRFFF,            !- Name",
        "    1.1552,                  !- Coefficient1 Constant",
        "    -0.1808,                 !- Coefficient2 x",
        "    0.0256,                  !- Coefficient3 x**2",
        "    0.5,                     !- Minimum Value of x",
        "    1.5;                     !- Maximum Value of x",

        "  Curve:Quadratic,",
        "    WindACPLFFPLR,           !- Name",
        "    0.85,                    !- Coefficient1 Constant",
        "    0.15,                    !- Coefficient2 x",
        "    0.0,                     !- Coefficient3 x**2",
        "    0.0,                     !- Minimum Value of x",
        "    1.0;                     !- Maximum Value of x",

        "  NodeList,",
        "    ZoneInlets,              !- Name",
        "    LIVING ZONE NoReheat Air Outlet Node;         !- Node 1 Name",

        "  NodeList,",
        "    Supply Air Temp Nodes,   !- Name",
        "    Heating Coil Air Inlet Node,  !- Node 1 Name",
        "    Air Loop Outlet Node;    !- Node 2 Name",

        "  BranchList,",
        "    Air Loop Branches,       !- Name",
        "    Air Loop Main Branch;    !- Branch 1 Name",

        "  Branch,",
        "    Air Loop Main Branch,    !- Name",
        "    ,                        !- Pressure Drop Curve Name",
        "    AirLoopHVAC:UnitarySystem,  !- Component 1 Object Type",
        "    DXAC Heat Pump 1,        !- Component 1 Name",
        "    Air Loop Inlet Node,     !- Component 1 Inlet Node Name",
        "    Air Loop Outlet Node;    !- Component 1 Outlet Node Name",

        "  AirLoopHVAC,",
        "    Typical Residential System,  !- Name",
        "    ,                        !- Controller List Name",
        "    Reheat System 1 Avail List,  !- Availability Manager List Name",
        "    1.18,                    !- Design Supply Air Flow Rate {m3/s}",
        "    Air Loop Branches,       !- Branch List Name",
        "    ,                        !- Connector List Name",
        "    Air Loop Inlet Node,     !- Supply Side Inlet Node Name",
        "    Return Air Mixer Outlet, !- Demand Side Outlet Node Name",
        "    Zone Equipment Inlet Node,  !- Demand Side Inlet Node Names",
        "    Air Loop Outlet Node;    !- Supply Side Outlet Node Names",

        "  AirflowNetwork:SimulationControl,",
        "    House AirflowNetwork,    !- Name",
        "    MultizoneWithDistribution,  !- AirflowNetwork Control",
        "    INPUT,                   !- Wind Pressure Coefficient Type",
        "    ExternalNode,            !- Height Selection for Local Wind Pressure Calculation",
        "    LOWRISE,                 !- Building Type",
        "    500,                     !- Maximum Number of Iterations {dimensionless}",
        "    ZeroNodePressures,       !- Initialization Type",
        "    1.0E-05,                 !- Relative Airflow Convergence Tolerance {dimensionless}",
        "    1.0E-06,                 !- Absolute Airflow Convergence Tolerance {kg/s}",
        "    -0.5,                    !- Convergence Acceleration Limit {dimensionless}",
        "    0.0,                     !- Azimuth Angle of Long Axis of Building {deg}",
        "    1.0;                     !- Ratio of Building Width Along Short Axis to Width Along Long Axis",

        "  AirflowNetwork:MultiZone:Zone,",
        "    LIVING ZONE,             !- Zone Name",
        "    Temperature,             !- Ventilation Control Mode",
        "    WindowVentSched,         !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    0.3,                     !- Minimum Venting Open Factor {dimensionless}",
        "    5.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    10.0,                    !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    GARAGE ZONE,             !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Zone,",
        "    ATTIC ZONE,              !- Zone Name",
        "    NoVent,                  !- Ventilation Control Mode",
        "    ,                        !- Ventilation Control Zone Temperature Setpoint Schedule Name",
        "    1.0,                     !- Minimum Venting Open Factor {dimensionless}",
        "    0.0,                     !- Indoor and Outdoor Temperature Difference Lower Limit For Maximum Venting Open Factor {deltaC}",
        "    100.0,                   !- Indoor and Outdoor Temperature Difference Upper Limit for Minimum Venting Open Factor {deltaC}",
        "    0.0,                     !- Indoor and Outdoor Enthalpy Difference Lower Limit For Maximum Venting Open Factor {deltaJ/kg}",
        "    300000.0;                !- Indoor and Outdoor Enthalpy Difference Upper Limit for Minimum Venting Open Factor {deltaJ/kg}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Living:West,             !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    WFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Living:Interior,         !- Surface Name",
        "    CR-1,                    !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Garage:FrontDoor,        !- Surface Name",
        "    CRcri,                   !- Leakage Component Name",
        "    NFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    EastRoof,                !- Surface Name",
        "    AtticLeak,               !- Leakage Component Name",
        "    SFacade,                 !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Living:Ceiling,          !- Surface Name",
        "    InterCondZoneLeak,       !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface,",
        "    Garage:Ceiling,          !- Surface Name",
        "    InterZoneLeak,           !- Leakage Component Name",
        "    ,                        !- External Node Name",
        "    1.0;                     !- Window/Door Opening Factor, or Crack Factor {dimensionless}",

        "  AirflowNetwork:MultiZone:ReferenceCrackConditions,",
        "    ReferenceCrackConditions,!- Name",
        "    20.0,                    !- Reference Temperature {C}",
        "    101325,                  !- Reference Barometric Pressure {Pa}",
        "    0.0;                     !- Reference Humidity Ratio {kgWater/kgDryAir}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CR-1,                    !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    CRcri,                   !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.667;                   !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    AtticLeak,               !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    InterZoneLeak,           !- Name",
        "    0.05,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:Surface:Crack,",
        "    InterCondZoneLeak,       !- Name",
        "    0.02,                    !- Air Mass Flow Coefficient at Reference Conditions {kg/s}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    NFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    NFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    EFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    EFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    SFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    SFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:ExternalNode,",
        "    WFacade,                 !- Name",
        "    1.524,                   !- External Node Height {m}",
        "    WFacade_WPCValue;        !- Wind Pressure Coefficient Curve Name",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientArray,",
        "    Every 30 Degrees,        !- Name",
        "    0,                       !- Wind Direction 1 {deg}",
        "    30,                      !- Wind Direction 2 {deg}",
        "    60,                      !- Wind Direction 3 {deg}",
        "    90,                      !- Wind Direction 4 {deg}",
        "    120,                     !- Wind Direction 5 {deg}",
        "    150,                     !- Wind Direction 6 {deg}",
        "    180,                     !- Wind Direction 7 {deg}",
        "    210,                     !- Wind Direction 8 {deg}",
        "    240,                     !- Wind Direction 9 {deg}",
        "    270,                     !- Wind Direction 10 {deg}",
        "    300,                     !- Wind Direction 11 {deg}",
        "    330;                     !- Wind Direction 12 {deg}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    NFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    0.60,                    !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.48;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    EFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.56;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    SFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.37,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    -0.42;                   !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:MultiZone:WindPressureCoefficientValues,",
        "    WFacade_WPCValue,        !- Name",
        "    Every 30 Degrees,        !- AirflowNetwork:MultiZone:WindPressureCoefficientArray Name",
        "    -0.56,                   !- Wind Pressure Coefficient Value 1 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 2 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 3 {dimensionless}",
        "    -0.37,                   !- Wind Pressure Coefficient Value 4 {dimensionless}",
        "    -0.42,                   !- Wind Pressure Coefficient Value 5 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 6 {dimensionless}",
        "    -0.56,                   !- Wind Pressure Coefficient Value 7 {dimensionless}",
        "    0.04,                    !- Wind Pressure Coefficient Value 8 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 9 {dimensionless}",
        "    0.60,                    !- Wind Pressure Coefficient Value 10 {dimensionless}",
        "    0.48,                    !- Wind Pressure Coefficient Value 11 {dimensionless}",
        "    0.04;                    !- Wind Pressure Coefficient Value 12 {dimensionless}",

        "  AirflowNetwork:Distribution:Node,",
        "    EquipmentInletNode,      !- Name",
        "    Zone Equipment Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    SplitterNode,            !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneSupplyNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneSupplyRegisterNode,  !- Name",
        "    LIVING ZONE NoReheat Air Outlet Node,         !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneOutletNode,          !- Name",
        "    Zone Outlet Node,        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    ZoneReturnNode,          !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MixerNode,               !- Name",
        "    ,                        !- Component Name or Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainReturnNode,          !- Name",
        "    Return Air Mixer Outlet, !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    MainInletNode,           !- Name",
        "    ,                        !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    FanInletNode,            !- Name",
        "    Air Loop Inlet Node,     !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    CoolingInletNode,        !- Name",
        "    Cooling Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    HeatingInletNode,        !- Name",
        "    Heating Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    SuppHeatingInletNode,    !- Name",
        "    SuppHeating Coil Air Inlet Node,  !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Node,",
        "    AirLoopOutletNode,       !- Name",
        "    Air Loop Outlet Node,    !- Component Name or Node Name",
        "    Other,                   !- Component Object Type or Node Type",
        "    3.0;                     !- Node Height {m}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    SupplyLeak,              !- Name",
        "    0.1,                     !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    60,                      !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:LeakageRatio,",
        "    ReturnLeak,              !- Name",
        "    0.05,                    !- Effective Leakage Ratio {dimensionless}",
        "    1.9,                     !- Maximum Flow Rate {m3/s}",
        "    15,                      !- Reference Pressure Difference {Pa}",
        "    0.65;                    !- Air Mass Flow Exponent {dimensionless}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainTruck,               !- Name",
        "    2.0,                     !- Duct Length {m}",
        "    0.4064,                  !- Hydraulic Diameter {m}",
        "    0.1297,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.3,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneSupply,              !- Name",
        "    10.0,                    !- Duct Length {m}",
        "    0.4064,                  !- Hydraulic Diameter {m}",
        "    0.1297,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.91,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.946792,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    5.018000,                !- Outside Convection Coefficient {W/m2-K}",
        "    25.090000;               !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneReturn,              !- Name",
        "    3.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.01,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    ZoneConnectionDuct,      !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.0,                     !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    0.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    MainReturn,              !- Name",
        "    1.0,                     !- Duct Length {m}",
        "    0.50,                    !- Hydraulic Diameter {m}",
        "    0.1963,                  !- Cross Section Area {m2}",
        "    0.0009,                  !- Surface Roughness {m}",
        "    0.5,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopReturn,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.00,                    !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    0.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Duct,",
        "    AirLoopSupply,           !- Name",
        "    0.1,                     !- Duct Length {m}",
        "    1.00,                    !- Hydraulic Diameter {m}",
        "    0.7854,                  !- Cross Section Area {m2}",
        "    0.0001,                  !- Surface Roughness {m}",
        "    0.00,                    !- Coefficient for Local Dynamic Loss Due to Fitting {dimensionless}",
        "    0.001226,                !- Heat Transmittance Coefficient (U-Factor) for Duct Wall Construction {W/m2-K}",
        "    0.0000001,               !- Overall Moisture Transmittance Coefficient from Air to Air {kg/m2}",
        "    0.006500,                !- Outside Convection Coefficient {W/m2-K}",
        "    0.032500;                !- Inside Convection Coefficient {W/m2-K}",

        "  AirflowNetwork:Distribution:Component:Fan,",
        "    Supply Fan 1,            !- Fan Name",
        "    Fan:SystemModel;         !- Supply Fan Object Type",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    ACDXCoil 1,              !- Coil Name",
        "    Coil:Cooling:DX:SingleSpeed,  !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.000;                   !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Supp Heating Coil 1,     !- Coil Name",
        "    Coil:Heating:Fuel,       !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.000;                   !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Component:Coil,",
        "    Heat Pump DX Heating Coil 1,  !- Coil Name",
        "    Coil:Heating:DX:SingleSpeed,  !- Coil Object Type",
        "    0.1,                     !- Air Path Length {m}",
        "    1.000;                   !- Air Path Hydraulic Diameter {m}",

        "  AirflowNetwork:Distribution:Linkage,",
        "    Main Link,               !- Name",
        "    EquipmentInletNode,      !- Node 1 Name",
        "    SplitterNode,            !- Node 2 Name",
        "    MainTruck,               !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneSupplyLink1,         !- Name",
        "    SplitterNode,            !- Node 1 Name",
        "    ZoneSupplyNode,          !- Node 2 Name",
        "    ZoneSupply,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneSupply1Link2,        !- Name",
        "    ZoneSupplyNode,          !- Node 1 Name",
        "    ZoneSupplyRegisterNode,  !- Node 2 Name",
        "    ZoneSupply,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneSupplyConnectionLink,!- Name",
        "    ZoneSupplyRegisterNode,  !- Node 1 Name",
        "    LIVING ZONE,             !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneReturnConnectionLink,!- Name",
        "    LIVING ZONE,             !- Node 1 Name",
        "    ZoneOutletNode,          !- Node 2 Name",
        "    ZoneConnectionDuct;      !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneReturn1Link,         !- Name",
        "    ZoneOutletNode,          !- Node 1 Name",
        "    ZoneReturnNode,          !- Node 2 Name",
        "    ZoneReturn,              !- Component Name",
        "    Attic Zone;              !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneReturn2Link,         !- Name",
        "    ZoneReturnNode,          !- Node 1 Name",
        "    MixerNode,               !- Node 2 Name",
        "    ZoneReturn,              !- Component Name",
        "    Garage Zone;             !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ReturnMixerLink,         !- Name",
        "    MixerNode,               !- Node 1 Name",
        "    MainReturnNode,          !- Node 2 Name",
        "    MainReturn,              !- Component Name",
        "    Garage Zone;             !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SystemReturnLink,        !- Name",
        "    MainReturnNode,          !- Node 1 Name",
        "    MainInletNode,           !- Node 2 Name",
        "    AirLoopReturn;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SystemInletLink,         !- Name",
        "    MainInletNode,           !- Node 1 Name",
        "    FanInletNode,            !- Node 2 Name",
        "    MainReturn,              !- Component Name",
        "    GARAGE ZONE;             !- Thermal Zone Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SupplyFanLink,           !- Name",
        "    FanInletNode,            !- Node 1 Name",
        "    CoolingInletNode,        !- Node 2 Name",
        "    Supply Fan 1;            !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    CoolingCoilLink,         !- Name",
        "    CoolingInletNode,        !- Node 1 Name",
        "    HeatingInletNode,        !- Node 2 Name",
        "    ACDXCoil 1;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    HeatingCoilLink,         !- Name",
        "    HeatingInletNode,        !- Node 1 Name",
        "    SuppHeatingInletNode,    !- Node 2 Name",
        "    Heat Pump DX Heating Coil 1;  !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    SuppHeatingCoilLink,     !- Name",
        "    SuppHeatingInletNode,    !- Node 1 Name",
        "    AirLoopOutletNode,       !- Node 2 Name",
        "    Supp Heating Coil 1;     !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    EquipmentAirLoopLink,    !- Name",
        "    AirLoopOutletNode,       !- Node 1 Name",
        "    EquipmentInletNode,      !- Node 2 Name",
        "    AirLoopSupply;           !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneSupplyLeakLink,      !- Name",
        "    ZoneSupplyNode,          !- Node 1 Name",
        "    Attic Zone,              !- Node 2 Name",
        "    SupplyLeak;              !- Component Name",

        "  AirflowNetwork:Distribution:Linkage,",
        "    ZoneReturnLeakLink,      !- Name",
        "    Garage Zone,             !- Node 1 Name",
        "    ZoneReturnNode,          !- Node 2 Name",
        "    ReturnLeak;              !- Component Name",

        "  AvailabilityManagerAssignmentList,",
        "    Reheat System 1 Avail List,  !- Name",
        "    AvailabilityManager:Scheduled,  !- Availability Manager 1 Object Type",
        "    Reheat System 1 Avail;   !- Availability Manager 1 Name",

        "  AvailabilityManager:Scheduled,",
        "    Reheat System 1 Avail,   !- Name",
        "    HVACAvailSched;          !- Schedule Name",

        "  ZoneHVAC:EquipmentConnections,",
        "    LIVING ZONE,             !- Zone Name",
        "    ZoneEquipment,           !- Zone Conditioning Equipment List Name",
        "    ZoneInlets,              !- Zone Air Inlet Node or NodeList Name",
        "    ,                        !- Zone Air Exhaust Node or NodeList Name",
        "    Zone Node,               !- Zone Air Node Name",
        "    Zone Outlet Node;        !- Zone Return Air Node or NodeList Name",

        "  ZoneHVAC:EquipmentList,",
        "    ZoneEquipment,           !- Name",
        "    SequentialLoad,          !- Load Distribution Scheme",
        "    ZoneHVAC:AirDistributionUnit,  !- Zone Equipment 1 Object Type",
        "    LIVINGZONENoReheat,           !- Zone Equipment 1 Name",
        "    1,                       !- Zone Equipment 1 Cooling Sequence",
        "    1,                       !- Zone Equipment 1 Heating or No-Load Sequence",
        "    ,                        !- Zone Equipment 1 Sequential Cooling Load Fraction",
        "    ;                        !- Zone Equipment 1 Sequential Heating Load Fraction",

        "  AirLoopHVAC:UnitarySystem,",
        "    DXAC Heat Pump 1,      !- Name",
        "    Load,                !- Control Type",
        "    LIVING ZONE,             !- Controlling Zone or Thermostat Location",
        "    ,                        !- Dehumidification Control Type",
        "    HVACAvailSched,    !- Availability Schedule Name",
        "    Air Loop Inlet Node,     !- Air Inlet Node Name",
        "    Air Loop Outlet Node,    !- Air Outlet Node Name",
        "    Fan:SystemModel,               !- Supply Fan Object Type",
        "    Supply Fan 1,            !- Supply Fan Name",
        "    BlowThrough,             !- Fan Placement",
        "    CyclingFanSchedule,      !- Supply Air Fan Operating Mode Schedule Name",
        "    Coil:Heating:DX:SingleSpeed,  !- Heating Coil Object Type",
        "    Heat Pump DX Heating Coil 1,  !- Heating Coil Name",
        "    1.0,                        !- DX Heating Coil Sizing Ratio",
        "    Coil:Cooling:DX:SingleSpeed,   !- Cooling Coil Object Type",
        "    ACDXCoil 1,                        !- Cooling Coil Name",
        "    ,                        !- Use DOAS DX Cooling Coil",
        "    ,                        !- Minimum Supply Air Temperature",
        "    ,                        !- Latent Load Control",
        "    Coil:Heating:Fuel,       !- Supplemental Heating Coil Object Type",
        "    Supp Heating Coil 1,     !- Supplemental Heating Coil Name",
        "    SupplyAirFlowRate,       !- Cooling Supply Air Flow Rate Method",
        "    1.18,                    !- Cooling Supply Air Flow Rate",
        "    ,                        !- Cooling Supply Air Flow Rate Per Floor Area",
        "    ,                        !- Cooling Fraction of Autosized Cooling Supply Air Flow Rate",
        "    ,                        !- Cooling Supply Air Flow Rate Per Unit of Capacity",
        "    SupplyAirFlowRate,       !- Heating Supply Air Flow Rate Method",
        "    1.18,                    !- Heating Supply Air Flow Rate",
        "    ,                        !- Heating Supply Air Flow Rate Per Floor Area",
        "    ,                        !- Heating Fraction of Autosized Cooling Supply Air Flow Rate",
        "    ,                        !- Heating Supply Air Flow Rate Per Unit of Capacity",
        "    SupplyAirFlowRate,       !- No Load Supply Air Flow Rate Method",
        "    0;                       !- No Load Supply Air Flow Rate {m3/s}",

        "  ZoneHVAC:AirDistributionUnit,",
        "    LIVINGZONENoReheat,         !- Name",
        "    Zone 1 NoReheat Air Outlet Node,  !- Air Distribution Unit Outlet Node Name",
        "    AirTerminal:SingleDuct:ConstantVolume:NoReheat,  !- Air Terminal Object Type",
        "    No Reheat LIVING ZONE;           !- Air Terminal Name",

        "  AirTerminal:SingleDuct:ConstantVolume:NoReheat,",
        "    No Reheat LIVING ZONE,           !- Name",
        "    HVACAvailSched,    !- Availability Schedule Name",
        "    LIVING ZONE NoReheat Air Inlet Node,  !- Air Inlet Node Name",
        "    LIVING ZONE NoReheat Air Outlet Node,  !- Air Outlet Node Name",
        "    1.18;                    !- Maximum Air Flow Rate {m3/s}",

        "  ZoneControl:Thermostat,",
        "    Zone Thermostat,         !- Name",
        "    LIVING ZONE,             !- Zone or ZoneList Name",
        "    Dual Zone Control Type Sched,  !- Control Type Schedule Name",
        "    ThermostatSetpoint:DualSetpoint,  !- Control 1 Object Type",
        "    Setpoints;               !- Control 1 Name",

        "  ThermostatSetpoint:DualSetpoint,",
        "    Setpoints,               !- Name",
        "    Dual Heating Setpoints,  !- Heating Setpoint Temperature Schedule Name",
        "    Dual Cooling Setpoints;  !- Cooling Setpoint Temperature Schedule Name",

        "  AirLoopHVAC:SupplyPath,",
        "    TermReheatSupplyPath,    !- Name",
        "    Zone Equipment Inlet Node,  !- Supply Air Path Inlet Node Name",
        "    AirLoopHVAC:ZoneSplitter,!- Component 1 Object Type",
        "    Zone Supply Air Splitter;!- Component 1 Name",

        "  AirLoopHVAC:ReturnPath,",
        "    TermReheatReturnPath,    !- Name",
        "    Return Air Mixer Outlet, !- Return Air Path Outlet Node Name",
        "    AirLoopHVAC:ZoneMixer,   !- Component 1 Object Type",
        "    Zone Return Air Mixer;   !- Component 1 Name",

        "  AirLoopHVAC:ZoneSplitter,",
        "    Zone Supply Air Splitter,!- Name",
        "    Zone Equipment Inlet Node,  !- Inlet Node Name",
        "    LIVING ZONE NoReheat Air Inlet Node;         !- Outlet 1 Node Name",

        "  AirLoopHVAC:ZoneMixer,",
        "    Zone Return Air Mixer,   !- Name",
        "    Return Air Mixer Outlet, !- Outlet Node Name",
        "    Zone Outlet Node;        !- Inlet 1 Node Name",

        "  Coil:Heating:Fuel,",
        "    Supp Heating Coil 1,     !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    NaturalGas,              !- Fuel Type",
        "    0.8,                     !- Burner Efficiency",
        "    25000,                   !- Nominal Capacity {W}",
        "    SuppHeating Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Air Loop Outlet Node;    !- Air Outlet Node Name",

        "  Coil:Cooling:DX:SingleSpeed,",
        "    ACDXCoil 1,              !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    21000,                   !- Gross Rated Total Cooling Capacity {W}",
        "    0.8,                     !- Gross Rated Sensible Heat Ratio",
        "    3.0,                     !- Gross Rated Cooling COP {W/W}",
        "    1.18,                    !- Rated Air Flow Rate {m3/s}",
        "    ,                        !- 2017 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    ,                        !- 2023 Rated Evaporator Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    Cooling Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    Heating Coil Air Inlet Node,  !- Air Outlet Node Name",
        "    WindACCoolCapFT,         !- Total Cooling Capacity Function of Temperature Curve Name",
        "    WindACCoolCapFFF,        !- Total Cooling Capacity Function of Flow Fraction Curve Name",
        "    WindACEIRFT,             !- Energy Input Ratio Function of Temperature Curve Name",
        "    WindACEIRFFF,            !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "    WindACPLFFPLR;           !- Part Load Fraction Correlation Curve Name",

        "  Coil:Heating:DX:SingleSpeed,",
        "    Heat Pump DX Heating Coil 1,  !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    21000,                   !- Gross Rated Heating Capacity {W}",
        "    2.75,                    !- Gross Rated Heating COP {W/W}",
        "    1.18,                    !- Rated Air Flow Rate {m3/s}",
        "    ,                        !- 2017 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    ,                        !- 2023 Rated Supply Fan Power Per Volume Flow Rate {W/(m3/s)}",
        "    Heating Coil Air Inlet Node,  !- Air Inlet Node Name",
        "    SuppHeating Coil Air Inlet Node,  !- Air Outlet Node Name",
        "    HPACHeatCapFT,           !- Heating Capacity Function of Temperature Curve Name",
        "    HPACHeatCapFFF,          !- Heating Capacity Function of Flow Fraction Curve Name",
        "    HPACHeatEIRFT,           !- Energy Input Ratio Function of Temperature Curve Name",
        "    HPACHeatEIRFFF,          !- Energy Input Ratio Function of Flow Fraction Curve Name",
        "    HPACCOOLPLFFPLR,         !- Part Load Fraction Correlation Curve Name",
        "    ,                        !- Defrost Energy Input Ratio Function of Temperature Curve Name",
        "    -5.0,                    !- Minimum Outdoor Dry-Bulb Temperature for Compressor Operation {C}",
        "    ,                        !- Outdoor Dry-Bulb Temperature to Turn On Compressor {C}",
        "    5.0,                     !- Maximum Outdoor Dry-Bulb Temperature for Defrost Operation {C}",
        "    200.0,                   !- Crankcase Heater Capacity {W}",
        "    10.0,                    !- Maximum Outdoor Dry-Bulb Temperature for Crankcase Heater Operation {C}",
        "    Resistive,               !- Defrost Strategy",
        "    TIMED,                   !- Defrost Control",
        "    0.166667,                !- Defrost Time Period Fraction",
        "    20000;                   !- Resistive Defrost Heater Capacity {W}",

        "  Fan:SystemModel,",
        "    Supply Fan 1,            !- Name",
        "    HVACAvailSched,          !- Availability Schedule Name",
        "    Air Loop Inlet Node,     !- Air Inlet Node Name",
        "    Cooling Coil Air Inlet Node,  !- Air Outlet Node Name",
        "    1.18,                    !- Design Maximum Air Flow Rate {m3/s}",
        "    Discrete,                !- Speed Control Method",
        "    0.0,                     !- Electric Power Minimum Flow Rate Fraction",
        "    400.0,                   !- Design Pressure Rise {Pa}",
        "    0.9,                   !- Motor Efficiency",
        "    1.0,                     !- Motor In Air Stream Fraction",
        "    674.29,                !- Design Electric Power Consumption {W}",
        "    TotalEfficiencyAndPressure,  !- Design Power Sizing Method",
        "    ,                        !- Electric Power Per Unit Flow Rate {W/(m3/s)}",
        "    ,                        !- Electric Power Per Unit Flow Rate Per Unit Pressure {W/((m3/s)-Pa)}",
        "    0.7,                 !- Fan Total Efficiency",
        "    ,                        !- Electric Power Function of Flow Fraction Curve Name",
        "    ,                        !- Night Ventilation Mode Pressure Rise {Pa}",
        "    ,                        !- Night Ventilation Mode Flow Fraction",
        "    ,                        !- Motor Loss Zone Name",
        "    ,                        !- Motor Loss Radiative Fraction",
        "    Fan Energy;              !- End-Use Subcategory",
    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataIPShortCut->lNumericFieldBlanks.allocate(1000);
    state->dataIPShortCut->lAlphaFieldBlanks.allocate(1000);
    state->dataIPShortCut->cAlphaFieldNames.allocate(1000);
    state->dataIPShortCut->cNumericFieldNames.allocate(1000);
    state->dataIPShortCut->cAlphaArgs.allocate(1000);
    state->dataIPShortCut->rNumericArgs.allocate(1000);
    state->dataIPShortCut->lNumericFieldBlanks = false;
    state->dataIPShortCut->lAlphaFieldBlanks = false;
    state->dataIPShortCut->cAlphaFieldNames = " ";
    state->dataIPShortCut->cNumericFieldNames = " ";
    state->dataIPShortCut->cAlphaArgs = " ";
    state->dataIPShortCut->rNumericArgs = 0.0;

    bool ErrorsFound = false;
    // Read objects
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetWindowGlassSpectralData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    Material::GetMaterialData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);
    SurfaceGeometry::GetGeometryParameters(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosBldgRotAppGonly = 1.0;
    state->dataSurfaceGeometry->SinBldgRotAppGonly = 0.0;
    SurfaceGeometry::GetSurfaceData(*state, ErrorsFound);
    EXPECT_FALSE(ErrorsFound);

    // Read AirflowNetwork inputs
    state->afn->get_input();

    state->dataScheduleMgr->Schedule(1).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(2).CurrentValue = 100.0;
    state->dataScheduleMgr->Schedule(3).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(4).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(5).CurrentValue = 0.1;
    state->dataScheduleMgr->Schedule(6).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(7).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(8).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(9).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(10).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(11).CurrentValue = 21.0;
    state->dataScheduleMgr->Schedule(12).CurrentValue = 25.0;
    state->dataScheduleMgr->Schedule(13).CurrentValue = 1.0;
    state->dataScheduleMgr->Schedule(14).CurrentValue = 1.0;

    state->afn->AirflowNetworkFanActivated = true;
    state->dataEnvrn->OutDryBulbTemp = -17.29025;
    state->dataEnvrn->OutHumRat = 0.0008389;
    state->dataEnvrn->OutBaroPress = 99063.0;
    state->dataEnvrn->WindSpeed = 4.9;
    state->dataEnvrn->WindDir = 270.0;

    for (i = 1; i <= 21; ++i) {
        state->afn->AirflowNetworkNodeSimu(i).TZ = 23.0;
        state->afn->AirflowNetworkNodeSimu(i).WZ = 0.0008400;
        if ((i >= 4 && i <= 7)) {
            state->afn->AirflowNetworkNodeSimu(i).TZ =
                DataEnvironment::OutDryBulbTempAt(*state, state->afn->AirflowNetworkNodeData(i).NodeHeight); // AirflowNetworkNodeData vals differ
            state->afn->AirflowNetworkNodeSimu(i).WZ = state->dataEnvrn->OutHumRat;
        }
    }

    DataZoneEquipment::GetZoneEquipmentData(*state);
    ZoneAirLoopEquipmentManager::GetZoneAirLoopEquipment(*state);
    SimAirServingZones::GetAirPathData(*state);

    state->dataAirLoop->AirLoopAFNInfo(1).LoopFanOperationMode = 1;
    state->dataAirLoop->AirLoopAFNInfo(1).LoopOnOffFanPartLoadRatio = 0.0;
    state->dataAirLoop->AirLoopAFNInfo(1).LoopSystemOnMassFlowrate = 1.23;
    state->afn->AirflowNetworkLinkageData(17).AirLoopNum = 1;
    state->dataLoopNodes->Node(4).MassFlowRate = 1.23;

    // Duct sizing test
    state->afn->simulation_control.autosize_ducts = true;
    state->afn->simulation_control.ductSizing.method = AirflowNetwork::DuctSizingMethod::PressureLoss;

    state->afn->simulation_control.ductSizing.factor = 1.0;
    state->afn->simulation_control.ductSizing.max_velocity = 10.0;
    state->afn->simulation_control.ductSizing.supply_trunk_pressure_loss = 5.0;
    state->afn->simulation_control.ductSizing.supply_branch_pressure_loss = 70.0;
    state->afn->simulation_control.ductSizing.return_trunk_pressure_loss = 3.0;
    state->afn->simulation_control.ductSizing.return_branch_pressure_loss = 4.0;

    state->dataZoneTempPredictorCorrector->zoneHeatBalance.allocate(3);
    for (auto &thisZoneHB : state->dataZoneTempPredictorCorrector->zoneHeatBalance) {
        thisZoneHB.MAT = 23.0;
        thisZoneHB.ZoneAirHumRat = 0.0008400;
    }

    state->dataZoneEquip->ZoneEquipList(1).EquipIndex(1) = 1;
    state->dataDefineEquipment->AirDistUnit(1).MassFlowRateTU = 1.23;
    state->afn->AirflowNetworkNodeData(8).EPlusNodeNum = 8;
    state->afn->AirflowNetworkNodeData(11).EPlusNodeNum = 1;
    state->afn->AirflowNetworkNodeData(12).EPlusNodeNum = 7;
    state->afn->AirflowNetworkNodeData(15).EPlusNodeNum = 9;
    state->afn->AirflowNetworkNodeData(17).EPlusNodeNum = 4;
    state->afn->AirflowNetworkNodeData(18).EPlusNodeNum = 5;
    state->afn->AirflowNetworkNodeData(19).EPlusNodeNum = 2;
    state->afn->AirflowNetworkNodeData(20).EPlusNodeNum = 11;
    state->afn->AirflowNetworkNodeData(21).EPlusNodeNum = 3;
    state->afn->AirflowNetworkNodeData(11).EPlusTypeNum = AirflowNetwork::iEPlusNodeType::ZIN;
    state->afn->AirflowNetworkNodeData(12).EPlusTypeNum = AirflowNetwork::iEPlusNodeType::ZOU;
    state->dataEnvrn->StdRhoAir = 1.2;
    state->afn->DisSysCompCVFData(1).FlowRate = 1.23;
    state->afn->SizeDucts();

    // Pressure loss method
    // Supply trunk
    EXPECT_NEAR(state->afn->DisSysCompDuctData(4).hydraulicDiameter, 0.531093, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(4).A, 0.221529, 0.0001);
    // Supply Branch
    EXPECT_NEAR(state->afn->DisSysCompDuctData(7).hydraulicDiameter, 0.453361, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(7).A, 0.161427, 0.0001);
    // Return branch
    EXPECT_NEAR(state->afn->DisSysCompDuctData(6).hydraulicDiameter, 0.522597, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(6).A, 0.214498, 0.0001);
    // Return trunk
    EXPECT_NEAR(state->afn->DisSysCompDuctData(3).hydraulicDiameter, 0.653543, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(3).A, 0.335458, 0.0001);

    // Maximum velocity method
    state->afn->simulation_control.ductSizing.method = AirflowNetwork::DuctSizingMethod::MaxVelocity;
    state->afn->SizeDucts();
    // Supply trunk
    EXPECT_NEAR(state->afn->DisSysCompDuctData(4).hydraulicDiameter, 0.361257, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(4).A, 0.102500, 0.0001);
    // Supply Branch
    EXPECT_NEAR(state->afn->DisSysCompDuctData(7).hydraulicDiameter, 0.361257, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(7).A, 0.102500, 0.0001);
    // Return branch
    EXPECT_NEAR(state->afn->DisSysCompDuctData(6).hydraulicDiameter, 0.361257, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(6).A, 0.102500, 0.0001);
    // Return trunk
    EXPECT_NEAR(state->afn->DisSysCompDuctData(3).hydraulicDiameter, 0.361257, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(3).A, 0.102500, 0.0001);

    // PressureLossWithMaximumVelocity method
    state->afn->simulation_control.ductSizing.method = AirflowNetwork::DuctSizingMethod::VelocityAndLoss;
    state->afn->simulation_control.ductSizing.max_velocity = 6.0;
    state->afn->SizeDucts();
    // Supply trunk
    EXPECT_NEAR(state->afn->DisSysCompDuctData(4).hydraulicDiameter, 0.531093, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(4).A, 0.221529, 0.0001);
    // Supply Branch with Max velocity
    EXPECT_NEAR(state->afn->DisSysCompDuctData(7).hydraulicDiameter, 0.466381, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(7).A, 0.170833, 0.0001);
    // Return branch
    EXPECT_NEAR(state->afn->DisSysCompDuctData(6).hydraulicDiameter, 0.522597, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(6).A, 0.214498, 0.0001);
    // Return trunk
    EXPECT_NEAR(state->afn->DisSysCompDuctData(3).hydraulicDiameter, 0.653543, 0.0001);
    EXPECT_NEAR(state->afn->DisSysCompDuctData(3).A, 0.335458, 0.0001);
}
} // namespace EnergyPlus
