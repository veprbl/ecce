//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector constructor
//
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"
#include "DD4hep/Printout.h"

#include "TGDMLParse.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)
{
  xml_det_t     x_det             = e;
  int           det_id            = x_det.id();
  string        det_name          = x_det.nameStr();
  Material      air               = description.air();

  // get the solids section for this detector
  xml_comp_t    x_solids         = x_det.child("solids");

  DetElement       sdet(det_name, det_id);
  Volume           motherVol = description.pickMotherVolume(sdet);

  std::string   string_rmin      = getAttrOrDefault<std::string>(x_det, _Unicode(rmin), "1780");
  std::string   string_rmax      = getAttrOrDefault<std::string>(x_det, _Unicode(rmax), "2660");
  std::string   string_length    = getAttrOrDefault<std::string>(x_det, _Unicode(length), "3160");

  double           rmin = (atof(string_rmin.c_str()))*dd4hep::mm;
  double           rmax = (atof(string_rmax.c_str()))*dd4hep::mm;
  double           length = (atof(string_length.c_str()))*dd4hep::mm;

  Tube             etube(rmin,rmax, length);
  Volume           envelope(det_name, etube, air);

  TGDMLParse p;
  TGeoVolume *plate_gdml = p.GDMLReadFile("HCAL_Chimney_Sector_Plate.gdml");
  Volume plate_v(plate_gdml);
  plate_v.import();
  TGeoVolume *half_plate_gdml = p.GDMLReadFile("HCAL_Chimney_Sector_Half_Plate.gdml");
  Volume half_plate_v(half_plate_gdml);
  half_plate_v.import();

  // Normal sectors
  for(int i=3; i<256; i++){
    PlacedVolume     sect_phv = envelope.placeVolume((i % 2) ? plate_v : half_plate_v, RotationZYX(i*(2*M_PI/256),0,0) );
    sect_phv.addPhysVolID("system", det_id);
    sect_phv.addPhysVolID("barrel", 0);
    sect_phv.addPhysVolID("sector", i);
  }

  sdet.setPlacement(motherVol.placeVolume(envelope));

  std::string   env_vis = getAttrOrDefault<std::string>(x_det, _Unicode(env_vis), "HcalBarrelEnvelopeVis"); 
  envelope.setAttributes(description, x_det.regionStr(), x_det.limitsStr(), env_vis);
  return sdet;
}

DECLARE_DETELEMENT(epic_oHcalBarrel, create_detector)
DECLARE_DETELEMENT(epic_HcalBarrel, create_detector)

