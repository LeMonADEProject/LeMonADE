/*--------------------------------------------------------------------------------
    ooo      L   attice-based  |
  o\.|./o    e   xtensible     | LeMonADE: An Open Source Implementation of the
 o\.\|/./o   Mon te-Carlo      |           Bond-Fluctuation-Model for Polymers
oo---0---oo  A   lgorithm and  |
 o/./|\.\o   D   evelopment    | Copyright (C) 2013-2015 by
  o/.|.\o    E   nvironment    | LeMonADE Principal Developers (see AUTHORS)
    ooo                        |
----------------------------------------------------------------------------------

This file is part of LeMonADE.

LeMonADE is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LeMonADE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LeMonADE.  If not, see <http://www.gnu.org/licenses/>.

--------------------------------------------------------------------------------*/

#ifndef LEMONADE_UTILITY_LATTICE_H
#define LEMONADE_UTILITY_LATTICE_H

#include <LeMonADE/utility/Vector3D.h>
/**
 * @class Lattice 
 * @brief is a simple multidimensional lattice with value type
 */

template <class LatticeType>
class Lattice<LatticeType>
{

public:
	Lattice():_boxX(0),_boxY(0),_boxZ(0),boxXm1(0),boxYm1(0),boxZm1(0),xPro(0),proXY(0),lattice(NULL) {};
	~Lattice();
	
	//!copy constructor 
	Lattice(const Lattice& LatticeSource);

 	Lattice& operator= (const Lattice &LatticeSource);

	//! Allocate memory for the lattice
	void setupLattice();

	//! Allocate memory for the lattice
	void setupLattice(uint32_t boxX, uint32_t boxY,uint32_t boxZ );


	//! Set lattice entries to 0
	void clearLattice();

	//delocate memory
        void deleteLattice();
	
	//! Move the value on the lattice to a new position. Delete the Value on the old position
	void moveOnLattice(const VectorInt3& oldPos, const VectorInt3& newPos);

	//! Move the value on the lattice to a new position. Delete the Value on the old position
	void moveOnLattice(const int xOldPos, const int yOldPos, const int zOldPos, const int xNewPos, const int yNewPos, const int zNewPos);

	//! Get the lattice value at a certain point
	LatticeType getLatticeEntry(const VectorInt3& pos) const;

	//! Get the lattice value at a certain point
	LatticeType getLatticeEntry(const int x, const int y, const int z) const;

	//! Set the value on a lattice point
	void setLatticeEntry(const VectorInt3& pos, LatticeType val);

	//! Set the value on a lattice point
	void setLatticeEntry(const int x, const int y, const int z, LatticeType val);



private:
  	//! Hold the value of lattice size in X
	uint32_t _boxX;

	//! Hold the value of lattice size in Y
	uint32_t _boxY;

	//! Hold the value of lattice size in Z
	uint32_t _boxZ;

	//! Hold the value of lattice size in X subtracting one for folding
	uint32_t boxXm1;

	//! Hold the value of lattice size in Y subtracting one for folding
	uint32_t boxYm1;

	//! Hold the value of lattice size in Z subtracting one for folding
	uint32_t boxZm1;

	//! Hold the value of for indexing the lattice (FeatureLattice: boxX; FeatureLatticePowerOfTwo: log(boxX,2))
	uint32_t xPro;

	//! Hold the value of for indexing the lattice (FeatureLattice: boxX*boxY; FeatureLatticePowerOfTwo: log(boxX,2)*log(boxY,2))
	uint32_t proXY;
  	/**
	 * @brief Linearized 3D lattice of type ValueType to 1D.
	 *
	 * @details Linearized 3D lattice of type \a ValueType to 1D by formula in:
	 * FeatureLattice: lattice[idx]=lattice[x+y*xPro+z*proXY]
	 * FeatureLatticePowerOfTwo: lattice[idx]=lattice[x+(y<<xPro)+(z<<proXY)]
	 */
	LatticeType* lattice;
};
/******************************************************************************/
/**
 * Member implementation
 */
/******************************************************************************/
template <class LatticeType>
Lattice< LatticeType >::~Lattice()
{
 this->deleteLattice();
};

template <class LatticeType>
void Lattice< LatticeType >::deleteLattice()
{
    // free memory
    if(lattice != NULL) delete[] lattice;
    lattice = NULL;
};

template <class LatticeType>
Lattice< LatticeType >::Lattice(const Lattice& LatticeSource)
{
	std::cout << "LatticeSource" << std::endl;

	_boxX = LatticeSource._boxX;
	_boxY = LatticeSource._boxY;
	_boxZ = LatticeSource._boxZ;

	boxXm1 = LatticeSource.boxXm1;
	boxYm1 = LatticeSource.boxYm1;
	boxZm1 = LatticeSource.boxZm1;

	xPro = LatticeSource.xPro;
	proXY = LatticeSource.proXY;

	lattice = new ValueType[_boxX*_boxY*_boxZ];
}

template <class LatticeType>
Lattice< LatticeType >&  Lattice< LatticeType >::operator = (const Lattice< LatticeType > &LatticeSource)
{

    // check for self-assignment by comparing the address of the
    // implicit object and the parameter
    if (this == &LatticeSource)
        return *this;

    uint64_t oldSize = _boxX*_boxY*_boxZ;
    uint64_t newSize = LatticeSource._boxX*LatticeSource._boxY*LatticeSource._boxZ;

    _boxX  = LatticeSource._boxX;
    _boxY  = LatticeSource._boxY;
    _boxZ  = LatticeSource._boxZ;

    boxXm1 = LatticeSource.boxXm1;
    boxYm1 = LatticeSource.boxYm1;
    boxZm1 = LatticeSource.boxZm1;

    xPro   = LatticeSource.xPro;
    proXY  = LatticeSource.proXY;

    if ( oldSize != newSize )
    {
        this->deleteLattice();
        lattice = new ValueType[_boxX*_boxY*_boxZ];
    }

    // do the copy
    // return the existing object
    return *this;
}
/**
 * This method allocates memory for the lattice and fill it with the native value of \a ValueType.
 */
template <class LatticeType>
void Lattice< LatticeType >::setupLattice()
{

	std::cout<<"setting up lattice...";

	// Allocate memory
	lattice = new ValueType[_boxX*_boxY*_boxZ];

	for(uint32_t i = 0; i < _boxX*_boxY*_boxZ; i++)
		lattice[i]=ValueType();

	std::cout<<"done with size " << (_boxX*_boxY*_boxZ*sizeof(ValueType)) << " bytes = " << (_boxX*_boxY*_boxZ*sizeof(ValueType)/(1024.0*1024.0)) << " MB for lattice" <<std::endl;

}

/**
 * This method allocates memory for the lattice and fill it with the native value of \a ValueType.
 */
template <class LatticeType>
void Lattice< LatticeType >::setupLattice(uint32_t boxX, uint32_t boxY,uint32_t boxZ )
{

	_boxX = boxX;
	_boxY = boxY;
	_boxZ = boxZ;

	this->boxXm1=this->_boxX-1;
	this->boxYm1=this->_boxY-1;
	this->boxZm1=this->_boxZ-1;

	// determine the shift values for first multiplication
	this->xPro=this->_boxX;

	// determine the shift values for second multiplication
	this->proXY=this->_boxX*this->_boxY;

	std::cout << "use bit shift for boxX: ("<< this->xPro << " ) = " << (this->xPro) << " = " << (this->_boxX) << std::endl;
	std::cout << "use bit shift for boxX*boxY: ("<< this->proXY << " ) = " << (this->proXY) << " = " << (this->_boxX*this->_boxY) << std::endl;


	std::cout<<"setting up lattice...";

	// Allocate memory
	lattice = new ValueType[_boxX*_boxY*_boxZ];

	for(uint32_t i = 0; i < _boxX*_boxY*_boxZ; i++)
		lattice[i]=ValueType();

	std::cout<<"done with size " << (_boxX*_boxY*_boxZ*sizeof(ValueType)) << " bytes = " << (_boxX*_boxY*_boxZ*sizeof(ValueType)/(1024.0*1024.0)) << " MB for lattice" <<std::endl;

}


/**
 * All lattice entries are set to the native value of \a ValueType (in most cases Zero).
 * This method only clears the lattice. It will not destroy nor recreate the array.
 */
template <class LatticeType>
void Lattice< LatticeType >::clearLattice()
{

	// initialize to native value (=0)
	for(uint32_t i = 0; i < _boxX*_boxY*_boxZ; i++)
		lattice[i]=ValueType();
}



/******************************************************************************/
/**
 * @details Move a lattice point to a new position. The data at the new position
 * is overwritten and the old position is set to ValueType() (most Zero).
 *
 * @param oldPos old position
 * @param newPos new position
 */
/******************************************************************************/
template<template<typename> class SpecializedClass, typename ValueType>
inline void FeatureLatticeBase<SpecializedClass<ValueType> >::moveOnLattice(const VectorInt3& oldPos, const VectorInt3& newPos)
{
	static_cast<SpecializedClass<ValueType>* >(this)->moveOnLattice(oldPos, newPos);
}

/******************************************************************************/
/**
 * @details Move a lattice point to a new position. The data at the new position
 * is overwritten and the old position is set to ValueType() (most Zero).
 *
 * @param[in] xOldPos x-coordinate of old position in absolute coordinates
 * @param[in] yOldPos y-coordinate of old position in absolute coordinates
 * @param[in] zOldPos z-coordinate of old position in absolute coordinates
 * @param[in] xNewPos x-coordinate of new position in absolute coordinates
 * @param[in] yNewPos y-coordinate of new position in absolute coordinates
 * @param[in] zNewPos z-coordinate of new position in absolute coordinates
 */
/******************************************************************************/

template<template<typename> class SpecializedClass, typename ValueType>
inline void FeatureLatticeBase<SpecializedClass<ValueType> >::moveOnLattice(const int xOldPos, const int yOldPos, const int zOldPos, const int xNewPos, const int yNewPos, const int zNewPos)
{
	static_cast<SpecializedClass<ValueType>* >(this)->moveOnLattice(xOldPos, yOldPos, zOldPos, xNewPos, yNewPos, zNewPos);
}

/**
 * Get the value stored on the lattice at coordinates given by VectorInt3 \a pos.
 * @param[in] pos specified position
 * @return \p ValueType value on the specified position \a pos
 */
template<template<typename> class SpecializedClass, typename ValueType>
inline ValueType FeatureLatticeBase<SpecializedClass<ValueType> >::getLatticeEntry(const VectorInt3& pos) const
{
	return static_cast<const SpecializedClass<ValueType>* >(this)->getLatticeEntry(pos);
}


/**
 * Get the value stored on the lattice at coordinates given by Cartesian x y z coordinates.
 * @param[in] x x-coordinate on the Cartesian lattice
 * @param[in] y y-coordinate on the Cartesian lattice
 * @param[in] z z-coordinate on the Cartesian lattice
 * @return \a ValueType value on the specified position at x y z
 */
template<template<typename> class SpecializedClass, typename ValueType>
inline ValueType FeatureLatticeBase<SpecializedClass<ValueType> >::getLatticeEntry(const int x, const int y, const int z) const
{
	return static_cast<const SpecializedClass<ValueType>* >(this)->getLatticeEntry(x, y, z);
}


/**
 * Set the value \a val on the lattice at coordinates given by \a pos.
 * @param[in] pos specified position
 * @param[in] val \e ValueType to set on \a pos
 */
template<template<typename> class SpecializedClass, typename ValueType>
inline void FeatureLatticeBase<SpecializedClass<ValueType> >::setLatticeEntry(const VectorInt3& pos, ValueType val)
{
	static_cast<SpecializedClass<ValueType>* >(this)->setLatticeEntry(pos, val);
}


/**
 * Set the value \a val on the lattice at coordinates given by \a pos.
 * @param[in] x x-coordinate on the Cartesian lattice
 * @param[in] y y-coordinate on the Cartesian lattice
 * @param[in] z z-coordinate on the Cartesian lattice
 * @param[in] val \e ValueType to set on \a pos
 */
template<template<typename> class SpecializedClass, typename ValueType>
inline void FeatureLatticeBase<SpecializedClass<ValueType> >::setLatticeEntry(const int x, const int y, const int z, ValueType val)
{
	static_cast<SpecializedClass<ValueType>* >(this)->setLatticeEntry(x, y, z, val);
}


#endif /* LEMONADE_UTILITY_LATTICE_H */