#include "VRML2Writer.hh"

// stl
#include <sstream>
#include <iostream>

// geant4
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4Polyhedron.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Point3D.hh"
#include "G4SystemOfUnits.hh"

#include "math.h"

namespace slic {

VRML2Writer::VRML2Writer(const ::std::string& filename) :
		_indentLevel(0) {
	_out.open(filename.c_str());
}

VRML2Writer::~VRML2Writer() {
	if (_out.is_open())
		_out.close();
}

void VRML2Writer::write() {
	writeLine("#VRML V2.0 utf8");
	processTopVolume(
			G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume());
}

void VRML2Writer::processTopVolume(G4VPhysicalVolume* wv) {
	writeLine("Group {");
	indent();
	writeLine("children [");
	indent();
	processPhysicalVolume(wv);
	unindent();
	writeLine("]");
	unindent();
	writeLine("}"); // Group
}

void VRML2Writer::processPhysicalVolume(G4VPhysicalVolume* pv) {
	writeLine("DEF " + pv->GetName() + " Transform {");
	indent();
	writeLine("children [");
	indent();

	// shape node for each physical volume
	const G4VisAttributes* vis = pv->GetLogicalVolume()->GetVisAttributes();
	bool show = true;
	if (vis && !vis->IsVisible())
		show = false;
	if (show) {
		writeLine("DEF " + pv->GetName() + " Shape {");
		indent();
		processVisualization(pv);
		processSolid(pv->GetLogicalVolume()->GetSolid());
		unindent();
		writeLine("}");
	}

	G4LogicalVolume* lv = pv->GetLogicalVolume();

	// Recurse through daughter volumes
	bool showDaughters = true;
	if (vis && vis->IsDaughtersInvisible())
		showDaughters = false;
	if (showDaughters) {
		for (int i = 0, ndau = lv->GetNoDaughters(); i < ndau; i++) {
			processPhysicalVolume(lv->GetDaughter(i));
		}
	}

	unindent();
	writeLine("]"); // children

	// translation
	const G4ThreeVector& pos = pv->GetTranslation();
	std::stringstream ss;
	ss << "translation " << pos.x() / m << " " << pos.y() / m << " " << pos.z() / m;
	writeLine(ss.str());

	// rotation
	const G4RotationMatrix* rot = pv->GetRotation();
	CLHEP::Hep3Vector axis(0, 0, 0);
	double angle = 0;
	if (rot != 0)
		rot->getAngleAxis(angle, axis);
	std::stringstream ss2;
	ss2 << "rotation " << axis.x() << " " << axis.y() << " " << axis.z() << " " << -angle;
	rot = 0;
	writeLine(ss2.str());

	unindent();
	writeLine("}"); // Transform        
}

void VRML2Writer::processVisualization(G4VPhysicalVolume* pv) {
	const G4VisAttributes* vis = pv->GetLogicalVolume()->GetVisAttributes();
	double r, g, b;
	r = g = b = 1.0;
	if (vis != 0) {
		const G4Colour& col = vis->GetColour();
		r = col.GetRed();
		g = col.GetGreen();
		b = col.GetBlue();
	}
	writeLine("appearance Appearance {");
	indent();
	writeLine("material Material {");
	indent();
	std::stringstream ss;
	ss << r << " " << g << " " << b;
	writeLine("diffuseColor " + ss.str());
	writeLine("transparency 0.0"); // use viewer like Deep View to get a wireframe or transparent view
	unindent();
	writeLine("}");
	unindent();
	writeLine("}");
}

void VRML2Writer::processSolid(G4VSolid* solid) {
	if (solid->GetEntityType() == "G4Box") {
		processBox((G4Box*) solid);
	} else {
		processPolyhedron(solid->GetPolyhedron());
	}
}

void VRML2Writer::processBox(G4Box* box) {
	writeLine("geometry Box {");
	indent();
	std::stringstream ss;
	ss << "size " << box->GetXHalfLength() * 2 / m << " " << box->GetYHalfLength() * 2 / m << " "
			<< box->GetZHalfLength() * 2 / m;
	writeLine(ss.str());
	unindent();
	writeLine("}");
}

// code taken from Geant4....
// G4MVRL2SceneHandlerFunc.icc - G4VRML2SCENEHANDLER::AddPrimitive(const G4Polyhedron& polyhedron)
void VRML2Writer::processPolyhedron(G4Polyhedron* polyhedron) {
	writeLine("geometry IndexedFaceSet {");
	indent();
	writeLine("coord Coordinate {");
	indent();
	writeLine("point [");
	indent();

	int i, j;
	for (i = 1, j = polyhedron->GetNoVertices(); j; j--, i++) {
		G4Point3D point = polyhedron->GetVertex(i);
		std::stringstream ss;
		ss << point.x() / m << " " << point.y() / m << " " << point.z() / m;
		writeLine(ss.str());
	}
	unindent();
	writeLine("]"); // point
	unindent();
	writeLine("}"); // coord

	writeLine("coordIndex [");
	indent();

	// facet loop
	int f;
	for (f = polyhedron->GetNoFacets(); f; f--) {
		// edge loop  
		bool notLastEdge;
		int index = -1, edgeFlag = 1;
		std::stringstream ss;
		do {
			notLastEdge = polyhedron->GetNextVertexIndex(index, edgeFlag);
			ss << index - 1 << ", ";
		} while (notLastEdge);
		ss << "-1, ";
		writeLine(ss.str());
	}

	unindent();
	writeLine("]");
	writeLine("solid FALSE");
	unindent();
	writeLine("}"); // IndexFaceSet
}

void VRML2Writer::writeLine(const std::string& text) {
	for (int i = 0; i < _indentLevel; i++) {
		_out << "    ";
	}
	_out << text << std::endl;
}

void VRML2Writer::indent() {
	++_indentLevel;
}

void VRML2Writer::unindent() {
	--_indentLevel;
}
}
