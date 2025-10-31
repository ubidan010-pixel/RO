#ifndef _ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_
#define _ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_

namespace ITF
{
    class Frise;
    class Actor;

    class ObjectControllerComponentUtils
    {
    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// compute controller matrices for functions below
        /// @param[out] _controllerMatrix : the resulting controller matrix.
        /// @param _actor
        static void computeControllerMatrixFromControllerActor(Matrix44 &_controllerMatrix,Actor *_actor);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// apply the matrix of the controller to the dynamic frieze
        /// @param[in] _controllerMatrix : this matrix transforms to local controller space when it was at its initial position, then translates and rotate to its current state
        /// @param _frise : the frieze. Must be dynamic, otherwise shows a warning
        /// @param _immediate : if btrue, the transform is applied immediately (we do not wait for next frieze update)
        static void applyMatrixToDynamicFrieze(const Matrix44 &_controllerMatrix, Frise *_frise, bbool _immediate);
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// apply the matrix of the controller
        /// @param _controllerMatrix : this matrix transforms to local controller space when it was at its initial position, then translates and rotate to its current state
        /// @param _pickable : the pickable
        static void applyMatrixToPickable(const Matrix44 &_controllerMatrix, Pickable *_pickable);
    };
}


#endif

