#pragma once

#include "Core/Camera.hh"
#include "Core/GameObject.hh"

namespace BrrEdit {
	class Player {
		public:
			Player(int argc, char** argv);
			~Player();

		private:
			void Update();
			void Render();
	
			Camera m_Camera;
			Room m_Room;
	};
}
