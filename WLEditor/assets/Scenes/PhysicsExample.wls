Scene: Untitled
Entities:
  - Entity: 1910123800261103880
    TagComponent:
      Tag: Ground
    TransformComponent:
      Position: [0, -1.75438118, 0]
      Rotation: [0, 0, 0]
      Scale: [8, 0.5, 1]
    SpriteRendererComponent:
      Color: [0.814671814, 0.804555357, 0.440363169, 1]
    Rigidbody2DComponent:
      BodyType: Static
      FixedRotation: false
    BoxCollider2DComponent:
      Offset: [0, 0]
      Size: [1, 1]
      Density: 1
      Friction: 0.200000003
      Restitution: 0
      RestitutionThreshold: 0.5
  - Entity: 8072431159575447130
    TagComponent:
      Tag: Squard
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    SpriteRendererComponent:
      Color: [1, 0.254498303, 0.254498303, 1]
    Rigidbody2DComponent:
      BodyType: Dynamic
      FixedRotation: false
    BoxCollider2DComponent:
      Offset: [0, 0]
      Size: [1, 1]
      Density: 1
      Friction: 0.200000003
      Restitution: 0
      RestitutionThreshold: 0.5
  - Entity: 18034685068954353523
    TagComponent:
      Tag: Camera
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    CameraComponent:
      Camera:
        ProjectionType: 1
        PerspectiveFOV: 0.785398185
        PerspectiveNear: 0.00999999978
        PerspectiveFar: 1000
        OrthographicSize: 10
        OrthographicNear: -1
        OrthographicFar: 1
      Primary: true
      FixedAspectRatio: false