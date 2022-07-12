#pragma onc
#include "Scene.h"

class CLEndScene final : public Scene
{
public:
	CLEndScene();
	virtual ~CLEndScene();
	CLEndScene(const CLEndScene& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

private:
	float m_width;
	float m_height;

	GameObject* m_CreditImage;
	GameObject* m_EscImage;

	float m_CreditTime;
};

