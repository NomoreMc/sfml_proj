#pragma once
#include <SFML/Graphics.hpp>
#include <string>

#include "Entity.h"
#include "EntityManager.h"

struct WindowConfig { int W, H, F, M; };
struct FontConfig { std::string FS; int S, FR, FG, FB; };
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game {
private:
	sf::RenderWindow m_window;		// 主窗口
	EntityManager m_entities;		// 实体管理器
	sf::Font m_font;
	sf::Text m_text;
	WindowConfig m_windowConfig = {0};
	FontConfig m_fontConfig;
	PlayerConfig m_playerConfig = { 0 };	// 玩家配置
	EnemyConfig m_enemyConfig = { 0 };		// 敌人配置
	BulletConfig m_bulletConfig = { 0 };	// 子弹配置

	int m_score = 0;
	int m_currentFrame = 0;			// 当前帧数
	int m_lastEnemySpawnTime = 0;	// 上一次敌人生成时间
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
	void sEnemySpawner();
	void sCollision();

	// helper function
	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> eneity);

public:
	Game(const std::string& configFilePath);

	// run the game
	void run();
};