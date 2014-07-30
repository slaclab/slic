#ifndef SLIC_VRML2WRITER_HH_
#define SLIC_VRML2WRITER_HH_ 1

#include <fstream>
#include <iostream>
#include <string>

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class G4VPhysicalVolume;
class G4Box;
class G4VSolid;
class G4Polyhedron;

namespace slic {
/**
 * @class VRML2Writer
 * @brief A simple, standalone VRML2 writer that dumps the detector geometry in a hierarchical format.
 */
class VRML2Writer {

public:

    /**
     * Class constructor.
     */
    VRML2Writer(const std::string& filename);

    /**
     * Class destructor.
     */
	~VRML2Writer();

	/**
	 * Write out VRML to the current file.
	 */
	void write();

private:

	/**
	 * Recursively process volumes from the top.
	 * @param[in] The top volume.
	 */
	void processTopVolume(G4VPhysicalVolume* volume);

	/**
	 * Process a physical volume.
	 * @param[in] volume The physical volume.
	 */
	void processPhysicalVolume(G4VPhysicalVolume* volume);

	/**
	 * Process the visualization settings of the volume.
	 * @param[in] volume The physical volume.
	 */
	void processVisualization(G4VPhysicalVolume* volume);

	/**
	 * Process a solid.
	 * @param[in] solid The G4VSolid.
	 */
	void processSolid(G4VSolid* solid);

	/**
	 * Process a box shape.
	 * @param[in] box The G4Box.
	 */
	void processBox(G4Box* box);

	/**
	 * Process a polyhedron.
	 * @param[in] poly The G4Polyhedron.
	 */
	void processPolyhedron(G4Polyhedron* poly);

	/**
	 * Write a line into the output.
	 * @param[in] line The line of output.
	 */
	void writeLine(const std::string& line);

	/**
	 * Indent once in the output.
	 */
	void indent();

	/**
	 * Remove a level of indentation.
	 */
	void unindent();

private:

	std::ofstream _out;
	int _indentLevel;
};
}

#endif
