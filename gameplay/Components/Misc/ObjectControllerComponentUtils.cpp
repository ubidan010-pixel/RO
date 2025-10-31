#include "precompiled_gameplay.h"

#ifndef _ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_
#include "gameplay/Components/Misc/ObjectControllerComponentUtils.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_


namespace ITF
{
    void ObjectControllerComponentUtils::computeControllerMatrixFromControllerActor(Matrix44 &_controllerMatrix, Actor *_actor)
    {
        Matrix44 rotationMatrix;
        Matrix44 initPosToZeroMatrix, invInitTransfo;
        Matrix44 transform;

        _actor->computePlacementMatrix(rotationMatrix, btrue);
		M44_setMatrixRotationZ(&initPosToZeroMatrix, _actor->getWorldInitialRot());
        const Vec3d &pos = _actor->getWorldInitialPos();
		initPosToZeroMatrix.setLineAt(3, pos.m_x, pos.m_y, pos.m_z, 1.f);
		M44_matrixInverse((f32*)invInitTransfo.fa16, (f32*)initPosToZeroMatrix.fa16);
        M44_mul4x4Out(&_controllerMatrix, &invInitTransfo, &rotationMatrix);

/*

        Matrix44 rotationMatrix;
        Matrix44 initPosToZeroMatrix;

        _actor->computePlacementMatrix(rotationMatrix, btrue);
        const Vec3d &initPos = _actor->getWorldInitialPos();
        M44_setMatrixTranslation(&initPosToZeroMatrix, -initPos.m_x, -initPos.m_y, -initPos.m_z);
        M44_mul4x4Out(&_controllerMatrix, &initPosToZeroMatrix, &rotationMatrix);
		*/
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void ObjectControllerComponentUtils::applyMatrixToDynamicFrieze(const Matrix44 &_controllerMatrix, Frise *_frieze, bbool _immediate)
    {
        //dynamic frieze
        if (_frieze->isDynamic())
        {
            _frieze->setDynamicTransform((GMatrix44&)_controllerMatrix);
            if (_immediate)
            {
                _frieze->applyDynamicTransform();
            }
        }
        else
        {
            ITF_WARNING(NULL, _frieze->isDynamic(),"destination frieze is not dynamic");
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void ObjectControllerComponentUtils::applyMatrixToPickable(const Matrix44 &_controllerMatrix, Pickable *_pickable)
    {
        //rotate actor position and direction
        Vec3d transformedPos;
        Vec3d initialPos = _pickable->getWorldInitialPos();
        M44_matrixCoordTransform(&transformedPos, &_controllerMatrix, &initialPos);
        f32 angle = _pickable->getWorldInitialRot();
        Vec3d direction;
        f32_CosSin(angle, &direction.m_x, &direction.m_y);
        direction.m_z = 0.f;
        Vec3d transformedDirection;
        M44_matrixVec3dTransform(&transformedDirection, &_controllerMatrix, &direction);
        angle = atan2f(transformedDirection.m_y, transformedDirection.m_x);

        _pickable->rotate(angle-_pickable->getAngle(),bfalse);
        _pickable->offsetPosition(transformedPos-_pickable->getPos(),bfalse);
    }


}