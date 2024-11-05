#pragma once
#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig{};
struct EnemyConfig{};
struct BulletConfig{};

class Game {
private:
	sf::RenderWindow m_window;		// ��Ϸ������
	EntityManager m_entities;		// ��Ϸʵ�����
	sf::Font m_font;
	sf::Text m_text;
	PlayerConfig m_playerConfig;	// �������
	EnemyConfig m_enemyConfig;		// ��������
	BulletConfig m_bulletConfig;	// �ӵ�����
	
	int m_score = 0;
	int m_currentFrame = 0;			// ��ǰ֡��
	int m_lastEnemySpawnTime = 0;	// ��һ�����ɵ��˵�ʱ��
	bool m_pause = false;
	bool m_running = true;

	std::shared_ptr<Entity> m_player;

	void init(const std::string& configFilePath);   // initialize the GameState with a config file path
	void setPaused(bool pause);

	// systems
	void sMovement();
	void sUserInput();
	void sLifeSpan();
	void sRender();
	void sEnemySpawn();
	void sCollision();

	// helper function
	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> eneity);

public:
	Game(const std::string &configFilePath);

	// ��Ϸ������
	void run();
};