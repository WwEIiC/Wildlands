using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Wildlands;

namespace Sandbox
{
    public class Camera : Entity
    {
        public Entity OtherEntity;
        public float DistanceFromPlayer = 5.0f;
        private Entity m_Player;

        void OnCraete()
        {
            m_Player = FindEntityByName("Player");
        }

        void OnUpdate(float ts)
        {
            if (m_Player != null)
            {
                Position = new Vector3(m_Player.Position.XY, DistanceFromPlayer);
            }
            else { m_Player = FindEntityByName("Player"); }

            float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.Up))
                velocity.Y = 0.2f * DistanceFromPlayer;
            else if (Input.IsKeyDown(KeyCode.Down))
                velocity.Y = -0.2f * DistanceFromPlayer;

            if (Input.IsKeyDown(KeyCode.Left))
                velocity.X = -0.2f * DistanceFromPlayer;
            else if (Input.IsKeyDown(KeyCode.Right))
                velocity.X = 0.2f * DistanceFromPlayer;

            velocity *= speed;

            Vector3 position = Position;
            position += velocity * ts;
            Position = position;
        }

    }
}