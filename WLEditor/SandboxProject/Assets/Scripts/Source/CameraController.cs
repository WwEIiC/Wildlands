using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Wildlands;

namespace Sandbox
{
    internal class CameraController : Entity
    {
        void OnUpdate(float ts)
        {
            float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y = 0.2f * Position.Z;
            else if (Input.IsKeyDown(KeyCode.S))
                velocity.Y = -0.2f * Position.Z;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X = -0.2f * Position.Z;
            else if (Input.IsKeyDown(KeyCode.D))
                velocity.X = 0.2f * Position.Z;

            if (Input.IsKeyDown(KeyCode.Q))
                velocity.Z += 10.0f;
            else if (Input.IsKeyDown(KeyCode.E))
                velocity.Z -= 10.0f;

            velocity *= speed;

            Vector3 position = Position;
            position += velocity * ts;
            Position = position;
        }

    }
}
