#pragma once

namespace test {

	class Test
	{
	public:
		Test() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRenderer() {}
		virtual void OnImGuiRenderer() {}
	};

}