include_directories (bullet-2.78/src) 

SET(BulletCollision_SRCS
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btDbvt.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btGhostObject.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/btUnionFind.cpp
	bullet-2.78/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btBoxShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btBox2dShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btCollisionShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btCompoundShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConcaveShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConeShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btCylinderShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btEmptyShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btShapeHull.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btSphereShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp
	bullet-2.78/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp
	bullet-2.78/src/BulletCollision/Gimpact/btContactProcessing.cpp
	bullet-2.78/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactBvh.cpp
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactShape.cpp
	bullet-2.78/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp
	bullet-2.78/src/BulletCollision/Gimpact/gim_box_set.cpp
	bullet-2.78/src/BulletCollision/Gimpact/gim_contact.cpp
	bullet-2.78/src/BulletCollision/Gimpact/gim_memory.cpp
	bullet-2.78/src/BulletCollision/Gimpact/gim_tri_collision.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp
)

SET(Root_HDRS
	bullet-2.78/src/btBulletCollisionCommon.h
)

SET(BroadphaseCollision_HDRS
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btAxisSweep3.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btBroadphaseInterface.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btDbvt.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btDispatcher.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btOverlappingPairCallback.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.h
	bullet-2.78/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.h
)
SET(CollisionDispatch_HDRS
	bullet-2.78/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionConfiguration.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionCreateFunc.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionObject.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCollisionWorld.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btGhostObject.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btManifoldResult.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/btUnionFind.h
	bullet-2.78/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.h
)
SET(CollisionShapes_HDRS
	bullet-2.78/src/BulletCollision/CollisionShapes/btBoxShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btBox2dShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btCapsuleShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btCollisionMargin.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btCollisionShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btCompoundShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConcaveShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConeShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexHullShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexInternalShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexPolyhedron.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvex2dShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btCylinderShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btEmptyShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btMaterial.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btMultiSphereShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btOptimizedBvh.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btShapeHull.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btSphereShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btStaticPlaneShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btStridingMeshInterface.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTetrahedronShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleBuffer.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleCallback.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleInfoMap.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleMesh.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleMeshShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btTriangleShape.h
	bullet-2.78/src/BulletCollision/CollisionShapes/btUniformScalingShape.h
)
SET(Gimpact_HDRS
	bullet-2.78/src/BulletCollision/Gimpact/btBoxCollision.h
	bullet-2.78/src/BulletCollision/Gimpact/btClipPolygon.h
	bullet-2.78/src/BulletCollision/Gimpact/btContactProcessing.h
	bullet-2.78/src/BulletCollision/Gimpact/btGenericPoolAllocator.h
	bullet-2.78/src/BulletCollision/Gimpact/btGeometryOperations.h
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactBvh.h
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactMassUtil.h
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.h
	bullet-2.78/src/BulletCollision/Gimpact/btGImpactShape.h
	bullet-2.78/src/BulletCollision/Gimpact/btQuantization.h
	bullet-2.78/src/BulletCollision/Gimpact/btTriangleShapeEx.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_array.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_basic_geometry_operations.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_bitset.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_box_collision.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_box_set.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_clip_polygon.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_contact.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_geom_types.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_geometry.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_hash_table.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_linear_math.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_math.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_memory.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_radixsort.h
	bullet-2.78/src/BulletCollision/Gimpact/gim_tri_collision.h
)
SET(NarrowPhaseCollision_HDRS
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btConvexCast.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btConvexPenetrationDepthSolver.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btDiscreteCollisionDetectorInterface.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btManifoldPoint.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btPointCollector.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btSimplexSolverInterface.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h
	bullet-2.78/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.h
)

SET(BulletCollision_HDRS
	${Root_HDRS}
	${BroadphaseCollision_HDRS}
	${CollisionDispatch_HDRS}
	${CollisionShapes_HDRS}
	${Gimpact_HDRS}
	${NarrowPhaseCollision_HDRS}
)

ADD_LIBRARY(bulletcollision ${BulletCollision_SRCS} ${BulletCollision_HDRS})


INSTALL(FILES ${Root_HDRS} DESTINATION include)
INSTALL(FILES ${BroadphaseCollision_HDRS} DESTINATION include/BulletCollision/BroadphaseCollision/)
INSTALL(FILES ${CollisionDispatch_HDRS} DESTINATION include/BulletCollision/CollisionDispatch/)
INSTALL(FILES ${CollisionShapes_HDRS} DESTINATION include/BulletCollision/CollisionShapes/)
INSTALL(FILES ${Gimpact_HDRS} DESTINATION include/BulletCollision/Gimpact/)
INSTALL(FILES ${NarrowPhaseCollision_HDRS} DESTINATION include/BulletCollision/NarrowPhaseCollision/)






SET(BulletDynamics_SRCS
	bullet-2.78/src/BulletDynamics/Character/btKinematicCharacterController.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp
	bullet-2.78/src/BulletDynamics/Dynamics/btContinuousDynamicsWorld.cpp
	bullet-2.78/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp
	bullet-2.78/src/BulletDynamics/Dynamics/btRigidBody.cpp
	bullet-2.78/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp
	bullet-2.78/src/BulletDynamics/Dynamics/Bullet-C-API.cpp
	bullet-2.78/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp
	bullet-2.78/src/BulletDynamics/Vehicle/btWheelInfo.cpp
)

SET(BulletDynamicsRoot_HDRS
	bullet-2.78/src/btBulletDynamicsCommon.h
	bullet-2.78/src/btBulletCollisionCommon.h
)
SET(ConstraintSolver_HDRS
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btConstraintSolver.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btContactConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btContactSolverInfo.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btHingeConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btJacobianEntry.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSliderConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSolverBody.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btSolverConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btTypedConstraint.h
	bullet-2.78/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.h
)
SET(Dynamics_HDRS
	bullet-2.78/src/BulletDynamics/Dynamics/btActionInterface.h
	bullet-2.78/src/BulletDynamics/Dynamics/btContinuousDynamicsWorld.h
	bullet-2.78/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h
	bullet-2.78/src/BulletDynamics/Dynamics/btDynamicsWorld.h
	bullet-2.78/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.h
	bullet-2.78/src/BulletDynamics/Dynamics/btRigidBody.h
)
SET(Vehicle_HDRS
	bullet-2.78/src/BulletDynamics/Vehicle/btRaycastVehicle.h
	bullet-2.78/src/BulletDynamics/Vehicle/btVehicleRaycaster.h
	bullet-2.78/src/BulletDynamics/Vehicle/btWheelInfo.h
)

SET(Character_HDRS
	bullet-2.78/src/BulletDynamics/Character/btCharacterControllerInterface.h
	bullet-2.78/src/BulletDynamics/Character/btKinematicCharacterController.h
)


SET(BulletDynamics_HDRS
	${BulletDynamicsRoot_HDRS}
	${ConstraintSolver_HDRS}
	${Dynamics_HDRS}
	${Vehicle_HDRS}
	${Character_HDRS}
)


ADD_LIBRARY(bulletdynamics ${BulletDynamics_SRCS} ${BulletDynamics_HDRS})

INSTALL(FILES ${BulletDynamicsRoot_HDRS} DESTINATION include/)
INSTALL(FILES ${ConstraintSolver_HDRS} DESTINATION include/BulletDynamics/ConstraintSolver/)
INSTALL(FILES ${Dynamics_HDRS} DESTINATION include/BulletDynamics/Dynamics/)
INSTALL(FILES ${Vehicle_HDRS} DESTINATION include/BulletDynamics/Vehicle/)
INSTALL(FILES ${Character_HDRS} DESTINATION include/BulletDynamics/Character/)


SET(BulletSoftBody_SRCS
	bullet-2.78/src/BulletSoftBody/btSoftBody.cpp
	bullet-2.78/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp
	bullet-2.78/src/BulletSoftBody/btSoftBodyHelpers.cpp
	bullet-2.78/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp
	bullet-2.78/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp
	bullet-2.78/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp
	bullet-2.78/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp
	bullet-2.78/src/BulletSoftBody/btDefaultSoftBodySolver.cpp

)

SET(BulletSoftBody_HDRS
	bullet-2.78/src/BulletSoftBody/btSoftBody.h
	bullet-2.78/src/BulletSoftBody/btSoftBodyData.h
	bullet-2.78/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.h
	bullet-2.78/src/BulletSoftBody/btSoftBodyHelpers.h
	bullet-2.78/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h
	bullet-2.78/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.h
	bullet-2.78/src/BulletSoftBody/btSoftRigidDynamicsWorld.h
	bullet-2.78/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.h
	bullet-2.78/src/BulletSoftBody/btSparseSDF.h

	bullet-2.78/src/BulletSoftBody/btSoftBodySolvers.h
	bullet-2.78/src/BulletSoftBody/btDefaultSoftBodySolver.h

	bullet-2.78/src/BulletSoftBody/btSoftBodySolverVertexBuffer.h
)



ADD_LIBRARY(bulletsoftbody  ${BulletSoftBody_SRCS} ${BulletSoftBody_HDRS})

INSTALL(FILES ${BulletSoftBody_HDRS} DESTINATION include/BulletSoftBody/)


SET(LinearMath_SRCS
	bullet-2.78/src/LinearMath/btAlignedAllocator.cpp
	bullet-2.78/src/LinearMath/btConvexHull.cpp
	bullet-2.78/src/LinearMath/btConvexHullComputer.cpp
	bullet-2.78/src/LinearMath/btGeometryUtil.cpp
	bullet-2.78/src/LinearMath/btQuickprof.cpp
	bullet-2.78/src/LinearMath/btSerializer.cpp
)

SET(LinearMath_HDRS
	bullet-2.78/src/LinearMath/btAabbUtil2.h
	bullet-2.78/src/LinearMath/btAlignedAllocator.h
	bullet-2.78/src/LinearMath/btAlignedObjectArray.h
	bullet-2.78/src/LinearMath/btConvexHull.h
	bullet-2.78/src/LinearMath/btConvexHullComputer.h
	bullet-2.78/src/LinearMath/btDefaultMotionState.h
	bullet-2.78/src/LinearMath/btGeometryUtil.h
	bullet-2.78/src/LinearMath/btHashMap.h
	bullet-2.78/src/LinearMath/btIDebugDraw.h
	bullet-2.78/src/LinearMath/btList.h
	bullet-2.78/src/LinearMath/btMatrix3x3.h
	bullet-2.78/src/LinearMath/btMinMax.h
	bullet-2.78/src/LinearMath/btMotionState.h
	bullet-2.78/src/LinearMath/btPoolAllocator.h
	bullet-2.78/src/LinearMath/btQuadWord.h
	bullet-2.78/src/LinearMath/btQuaternion.h
	bullet-2.78/src/LinearMath/btQuickprof.h
	bullet-2.78/src/LinearMath/btRandom.h
	bullet-2.78/src/LinearMath/btScalar.h
	bullet-2.78/src/LinearMath/btSerializer.h
	bullet-2.78/src/LinearMath/btStackAlloc.h
	bullet-2.78/src/LinearMath/btTransform.h
	bullet-2.78/src/LinearMath/btTransformUtil.h
	bullet-2.78/src/LinearMath/btVector3.h
)

ADD_LIBRARY(bulletlinearmath ${LinearMath_SRCS} ${LinearMath_HDRS})

INSTALL(FILES ${LinearMath_HDRS} DESTINATION include/LinearMath/)



INSTALL(TARGETS bulletcollision bulletdynamics bulletsoftbody bulletlinearmath
  ARCHIVE DESTINATION lib/
)