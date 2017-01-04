#pragma once

#include <functional>
#include <deque>

class Game
{
public:
   Game(Game const &) = delete;
   Game(Game &&) = delete;
   Game& operator=(Game const &) = delete;
   Game& operator=(Game &&) = delete;

   using Size = size_t;

   struct Point
   {
      Size x = 0;
      Size y = 0;
      Point() = default;
      Point(Size x, Size y) : x(x), y(y) {}
      inline bool operator ==(Point const & rhs) const
      {
         return this == &rhs || x == rhs.x && y == rhs.y;
      }

      inline bool operator !=(Point const & rhs) const
      {
         return !(*this == rhs);
      }
   };
   using PointCRef = Point const &;
   using Points = std::deque<Point>;
   using PointsCRef = Points const &;

   using CollisionHandler = std::function<void(void)>;
   using VictoryHandler = std::function<void(void)>;

   enum class Direction
   {
      Up = 1,               // 0001
      Down = (Up << 1) | 1, // 0011
      Left = Up << 2,       // 0100
      Right = Down << 2,    // 1100
   };

   static Game & instance();

   inline Size fieldHeight() const
   {
      return m_height;
   }

   inline Size fieldWidth() const
   {
      return m_width;
   }

   inline Size squareSize() const
   {
      return m_squareSize;
   }

   void singleStep();
   void turn(Direction direction);

   inline PointCRef food() const
   {
      return m_food;
   }
   
   inline PointsCRef snake() const
   {
      return m_snake;
   }

   inline bool isCollision() const
   {
      return m_isCollision;
   }

   inline Point collisionPoint() const
   {
      return m_collisionPoint;
   }

   inline bool isVictory() const
   {
      return m_isVictory;
   }

   void setCollisionHandler(CollisionHandler handler);
   void setVictoryHandler(VictoryHandler handler);

   void reset();

private:
   Game();
   ~Game();

   void onCollision(Point const & collisionPoint);
   void placeFood();

   Size m_height = 32;
   Size m_width = 32;
   Size m_squareSize = 16;

   Size m_startBodySize = 3;

   bool m_isCollision = false;
   Point m_collisionPoint;
   CollisionHandler m_collisionHandler;

   bool m_isVictory = false;
   VictoryHandler m_victoryHandler;

   Point m_food;
   Points m_snake;

   Direction m_direction = Direction::Up;
   bool m_backToFront = false;
};
