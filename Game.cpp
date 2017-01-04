#include "stdafx.h"
#include "Game.h"

#include <algorithm>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_set>
#include <vector>

Game::Game()
{
   reset();
}

void Game::reset()
{
   Size const startX = m_width / 2;
   Size const startY = m_height - 1;
   m_snake.clear();
   m_isCollision = false;
   m_backToFront = false;
   for (auto i = 0; i < m_startBodySize; ++i)
   {
      m_snake.emplace_back(startX, startY - i);
   }
   srand(unsigned(time(NULL)));
   placeFood();
}

Game::~Game()
{
}

Game & Game::instance()
{
   static Game s_instance;  // Since C++11 it is a thread-safe singleton initialization
   return s_instance;
}

void Game::singleStep()
{

   assert(m_height);
   assert(m_width);
   if(m_snake.size() == 0)
   {
      assert(!"Snake body is empty!");
   }

   if(m_isCollision)
      return;

   Point head = m_backToFront ? *m_snake.begin() : *m_snake.rbegin();
   Point tail = m_backToFront ? *m_snake.rbegin() : *m_snake.begin();

   // Checking for a collision with the walls
   switch (m_direction)
   {
   case Direction::Up:
      if (head.y == 0)  // collision with the top border
      {
         onCollision(head);
         return;
      }
      break;
   case Direction::Down:
      if (head.y == m_height - 1)  // collision with the border border
      {
         onCollision(head);
         return;
      }
      break;
   case Direction::Left:
      if (head.x == 0)  // collision with the left border
      {
         onCollision(head);
         return;
      }
      break;
   case Direction::Right:
      if (head.x == m_width - 1)  // collision with the right border
      {
         onCollision(head);
         return;
      }
      break;
   }

   Point nextHead = [&](Direction direction) -> Point {
      switch (direction)
      {
      case Direction::Up:
         return{ head.x, head.y - 1 };
      case Direction::Down:
         return{ head.x, head.y + 1 };
      case Direction::Left:
         return{ head.x - 1, head.y };
      case Direction::Right:
         return{ head.x + 1, head.y };
      }
      return head;
   }(m_direction);

   // Checking for collisions with the body
   if (nextHead != tail &&  // It's ok when nextHead == tail, as we are gonna remove the tail
      // The complexity is O(n). Improve it? Later, on demand!
      std::find(std::begin(m_snake), std::end(m_snake), nextHead) != std::end(m_snake)
      )
   {
      onCollision(head);
      return;
   }

   // No collisions, phew!
   if(m_backToFront)
   {
      m_snake.push_front(nextHead);
   }
   else
   {
      m_snake.push_back(nextHead);
   }
   if (nextHead != m_food)
   {
      if(m_backToFront)
      {
         m_snake.pop_back();
      }
      else
      {
         m_snake.pop_front();
      }
      return;
   }

   // Yummy! Gimme more!
   placeFood();
}

void Game::onCollision(Point const & collisionPoint)
{
   m_isCollision = true;
   m_collisionPoint = collisionPoint;
   if(m_collisionHandler)
   {
      m_collisionHandler();
   }
}

namespace std {
   template <>
   struct hash<Game::Point>
   {
      using argument_type = Game::Point;
      using result_type = size_t;

      result_type operator()(argument_type const & t) const
      {
         auto hasher = std::hash<Game::Size>();
         auto hx = hasher(t.x);
         auto hy = hasher(t.y);
         return hx ^ (hy << 1);
      }
   };
}

void Game::placeFood()
{
   // Naive implementation, needs optimizing in future
   std::vector<Point> freePoints;
   freePoints.reserve(m_height * m_width - m_snake.size());  // why not?
   std::unordered_set<Point> const snakePoints(std::cbegin(m_snake), std::cend(m_snake));
   for(Size x = 0; x < m_width; ++x)
   {
      for(Size y = 0; y < m_height; ++y)
      {
         Point point {x, y};
         if(snakePoints.find(point) == std::end(snakePoints))
         {
            freePoints.push_back(point);  // std::move? Nope, no sense
         }
      }
   }

   if(!freePoints.size())
   {
      // No free points left, victory!
      m_isVictory = true;
      // m_victoryHandler && m_victoryHandler();
      if(m_victoryHandler)
      {
         m_victoryHandler();
      }
      return;
   }
   // picking a random point from the freePoints
   m_food = freePoints.at(rand() % freePoints.size());
}

void Game::turn(Direction direction)
{
   if(m_direction == direction)
      return;
   if(int(m_direction) & int(direction))
      m_backToFront = !m_backToFront;
   m_direction = direction;
}

void Game::setCollisionHandler(CollisionHandler handler)
{
   m_collisionHandler = handler;
}

void Game::setVictoryHandler(VictoryHandler handler)
{
   m_victoryHandler = handler;
}
