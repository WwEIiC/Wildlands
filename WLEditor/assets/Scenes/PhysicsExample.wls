Scene: Untitled
Entities:
  - Entity: 8355822223596644370
    TagComponent:
      Tag: Ground
    TransformComponent:
      Position: [-1.29717219, -1.38396847, -0.0398894697]
      Rotation: [0, 0, -0.456855774]
      Scale: [8.00000095, 0.49999994, 0.999999762]
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
  - Entity: 1023539327931184868
    TagComponent:
      Tag: Circle
    TransformComponent:
      Position: [0.947273612, 0.21952635, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    CircleRendererComponent:
      Color: [0.316184908, 0.505977273, 0.779922783, 1]
      Thickness: 1
      Fade: 0.00499999989
    Rigidbody2DComponent:
      BodyType: Dynamic
      FixedRotation: false
    CircleCollider2DComponent:
      Offset: [0, 0]
      Radius: 0.5
      Density: 1
      Friction: 0.200000003
      Restitution: 0.699999988
      RestitutionThreshold: 0.5
  - Entity: 1910123800261103880
    TagComponent:
      Tag: Ground2
    TransformComponent:
      Position: [3.46064782, -1.06385446, 0.0864449963]
      Rotation: [0, 0, 0.599386036]
      Scale: [7.99999809, 0.499999881, 0.999999762]
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
      Position: [0, 1.29421043, 0]
      Rotation: [0, 0, 0]
      Scale: [0.639999986, 0.639999986, 0.639999986]
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
      Restitution: 0.800000012
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