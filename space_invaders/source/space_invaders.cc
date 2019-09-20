// space_invaders.cc

#include "space_invaders.h"

#define LOCAL_HOST 127, 0, 0, 1

namespace gamma
{
	game_base* create_game(string& caption, video_mode& mode)
	{
		caption = "SPACE INVADERS";
		mode = video_mode(1024, 512);
		return new uu::space_invaders;
	}
}

namespace uu
{
	namespace
	{
		struct contact
		{
			entity* a_;
			entity* b_;
			vector2 position_;
		};

		void check_collision(bullets& b, invaders& i, dynamic_array<contact>& c)
		{
			for (auto& bullet : b.entity_)
			{
				if (!bullet.visible_)
				{
					continue;
				}

				for (auto& invader : i.entity_)
				{
					if (!invader.visible_)
					{
						continue;
					}

					if (collider::overlap(bullet.collider_, invader.collider_))
					{
						bullet.visible_ = false;
						invader.visible_ = false;

						contact cc;
						cc.a_ = &bullet;
						cc.b_ = &invader;
						cc.position_ = invader.collider_.min();
						c.push_back(cc);

						break;
					}
				}
			}
		}

		void check_collision(bullets& bu, blocks& bl, dynamic_array<contact>& c)
		{
			for (int bullet_index = 0; bullet_index < _countof(bu.entity_); bullet_index++)
			{
				entity& bullet = bu.entity_[bullet_index];
				if (!bullet.visible_)
				{
					continue;
				}

				for (int block_index = 0; block_index < _countof(bl.entity_); block_index++)
				{
					entity& block = bl.entity_[block_index];
					if (!block.visible_)
					{
						continue;
					}

					if (collider::overlap(bullet.collider_, block.collider_))
					{
						bullet.visible_ = false;
						bl.health_[block_index]--;
						if (!bl.health_[block_index])
						{
							block.visible_ = false;
						}

						contact cc;
						cc.a_ = &bullet;
						cc.b_ = &block;
						cc.position_ = bullet.collider_.min() + vector2(0.0f, -24.0f * bu.direction_[bullet_index].x_);
						c.push_back(cc);

						break;
					}
				}
			}
		}

		void check_collision(bullets& b, spaceship& ship, dynamic_array<contact>& c)
		{
			for (auto& bullet : b.entity_)
			{
				if (!bullet.visible_)
				{
					continue;
				}

				if (collider::overlap(bullet.collider_, ship.entity_.collider_))
				{
					bullet.visible_ = false;

					contact cc;
					cc.a_ = &bullet;
					cc.b_ = &ship.entity_;
					cc.position_ = bullet.collider_.min() - vector2(0.0f, 26.0f);
					c.push_back(cc);

					break;
				}
			}
		}
	} // !anon

	constexpr int64 fire_rate_ms = 750;
	constexpr int64 send_interval = 100;

	space_invaders::space_invaders()
		: sprite_sheet_(sprites_)
		, state_(GAME_STATE_INIT)
		, invaders_left_({ 200.0f, 10.0f }, { -1.0f, 1.0f })
		, invaders_right_({ 1024.0f - 392.0f, 118.0f }, { 1.0f, -1.0f })
		, ship_left_({ 10.0f, 512.0f * 0.5f - 16.0f }, { 32.0f, 24.0f })
		, ship_right_({ 1024.0f - 48.0f, 512.0f * 0.5f - 16.0f }, { -32.0f, 24.0f })
		, blocks_left_({ 1024, 512 })
		, blocks_right_({ 1024, 512 })
		, connection_pair_(false, false)
		, is_host_(false)
		, send_timer_(send_interval)
	{
	}

	space_invaders::~space_invaders()
	{
	}

	bool space_invaders::enter()
	{
		if (!network::init())
		{
			auto errcode = network::error::get_error();
			return !system::message_box("Could not open socket!\nErrorCode: %d\nErrorMessage: %s",
										errcode, network::error::as_string(errcode));
		}

		if (!sprites_.create_from_file("assets/sprites.png"))
		{
			return false;
		}

		// note: sprite sheets
		sprite_sheet_.add(BULLET, { 18.0f, 110.0f,   6.0f,  1.0f });
		sprite_sheet_.add(EXPLOSION, { 18.0f, 113.0f,   8.0f, 13.0f });
		sprite_sheet_.add(LEFT_BASE_DMG0, { 0.0f,   0.0f,  16.0f, 22.0f });
		sprite_sheet_.add(LEFT_BASE_DMG1, { 0.0f,  22.0f,  16.0f, 22.0f });
		sprite_sheet_.add(LEFT_BASE_DMG2, { 0.0f,  44.0f,  16.0f, 22.0f });
		sprite_sheet_.add(LEFT_BASE_DMG3, { 0.0f,  66.0f,  16.0f, 22.0f });
		sprite_sheet_.add(LEFT_BASE_DMG4, { 0.0f,  88.0f,  16.0f, 22.0f });
		sprite_sheet_.add(LEFT_ENEMY_1, { 18.0f,   0.0f,   8.0f, 12.0f });
		sprite_sheet_.add(LEFT_ENEMY_2, { 18.0f,  32.0f,   8.0f, 12.0f });
		sprite_sheet_.add(LEFT_ENEMY_3, { 18.0f,  64.0f,   8.0f, 12.0f });
		sprite_sheet_.add(LEFT_PLAYER, { 18.0f,  94.0f,   8.0f, 13.0f });
		sprite_sheet_.add(RIGHT_BASE_DMG0, { 16.0f,   0.0f, -16.0f, 22.0f });
		sprite_sheet_.add(RIGHT_BASE_DMG1, { 16.0f,  22.0f, -16.0f, 22.0f });
		sprite_sheet_.add(RIGHT_BASE_DMG2, { 16.0f,  44.0f, -16.0f, 22.0f });
		sprite_sheet_.add(RIGHT_BASE_DMG3, { 16.0f,  66.0f, -16.0f, 22.0f });
		sprite_sheet_.add(RIGHT_BASE_DMG4, { 16.0f,  88.0f, -16.0f, 22.0f });
		sprite_sheet_.add(RIGHT_ENEMY_1, { 26.0f,   0.0f,  -8.0f, 12.0f });
		sprite_sheet_.add(RIGHT_ENEMY_2, { 26.0f,  32.0f,  -8.0f, 12.0f });
		sprite_sheet_.add(RIGHT_ENEMY_3, { 26.0f,  64.0f,  -8.0f, 12.0f });
		sprite_sheet_.add(RIGHT_PLAYER, { 26.0f,  94.0f,  -8.0f, 13.0f });

		// note: initialize entities
		bullets_.reset(sprite_sheet_, sprites_);
		explosions_.reset(sprite_sheet_, sprites_);
		invaders_left_.reset(sprite_sheet_, sprites_, true);
		invaders_right_.reset(sprite_sheet_, sprites_, false);
		ship_left_.reset(sprite_sheet_, sprites_, true);
		ship_right_.reset(sprite_sheet_, sprites_, false);
		blocks_left_.reset(sprite_sheet_, sprites_, LEFT_BASE_DMG0);
		blocks_right_.reset(sprite_sheet_, sprites_, RIGHT_BASE_DMG0);

		return true;
	}

	void space_invaders::exit()
	{
		sprites_.destroy();
		network::shut();
	}

	bool space_invaders::update(const time& dt, const keyboard& kb)
	{
		if (kb.is_released(KEYCODE_ESCAPE))
		{
			return false;
		}

		if (state_ == GAME_STATE_INIT)
		{
			ip_address local;
			local.set_port(32100);
			if (!socket_.open(local))
			{
				auto errcode = network::error::get_error();
				return !system::message_box("Could not open socket!\nErrorCode: %d\nErrorMessage: %s",
											errcode, network::error::as_string(errcode));
			}

			//remote_.set_host(LOCAL_HOST);
			remote_.set_host(192, 168, 1, 120);
			remote_.set_port(32100);

			state_ = GAME_STATE_CONNECTING;
		}

		else if (state_ == GAME_STATE_CONNECTING)
		{

			if (kb.is_pressed(KEYCODE_RETURN))
			{
				send_connection_request();
				if (!connection_pair_.second)
					is_host_ = true;
			}

			uint8 array[1400];
			gamma::byte_stream stream(sizeof(array), array);
			if (socket_.recv_from(remote_, stream))
			{
				if (receive_connection_request(stream))
				{
					send_connection_response();
					connection_pair_.second = true;
				}

				if (receive_connection_response(stream))
					connection_pair_.first = true;
			}

			if (connection_pair_.first && connection_pair_.second)
				state_ = GAME_STATE_PLAY;
		}

		else if (state_ == GAME_STATE_PLAY)
		{

			send_timer_ = send_timer_ - dt;
			if (send_timer_.as_milliseconds() < 0.f)
			{
				send_timer_ = time(send_interval);
				// send
				uu::message_input_buffer message_input_buffer(input_buffer_);
				send_input_buffer(message_input_buffer);
				input_buffer_.clear();
			}

			uu::message_input_buffer OUT_message_input_buffer;
			while (receive_input_buffer(OUT_message_input_buffer))
			{
				for (int i = 0; i < OUT_message_input_buffer.size_; ++i)
				{
					input input = OUT_message_input_buffer.input_buffer_[i];

					if (input.dt_ == 0)
						break;

					ship_right_.direction_ = {};
					if (input.has_up())
					{
						ship_right_.direction_.y_ -= 1.0f;
					}
					if (input.has_down())
					{
						ship_right_.direction_.y_ += 1.0f;
					}
					if (input.has_space())
					{
						vector2 pos = ship_right_.entity_.position_ + ship_right_.offset_;
						bullets_.spawn(pos, { -1.0f, 0.0f });
					}

					time buffer_dt(input.dt_);
					bullets_.update(buffer_dt);
					invaders_right_.update(buffer_dt);
					ship_right_.update(buffer_dt);
					blocks_right_.update(sprite_sheet_);
				}
			}

			//uu::message_input OUT_input_message;
			//if (receive_input(OUT_input_message))
			//{
			   // // note: remote player
			   // ship_right_.direction_ = {};
			   // if (OUT_input_message.has_up())
			   // {
				  //  ship_right_.direction_.y_ -= 1.0f;
			   // }
			   // if (OUT_input_message.has_down())
			   // {
				  //  ship_right_.direction_.y_ += 1.0f;
			   // }
			   // if (OUT_input_message.has_space())
			   // {
				  //  vector2 pos = ship_right_.entity_.position_ + ship_right_.offset_;
				  //  bullets_.spawn(pos, { -1.0f, 0.0f });
			   // }
			//}


			bool up = false;
			bool down = false;
			bool space = false;
			// note: local player
			ship_left_.direction_ = {};
			if (kb.is_down(KEYCODE_W))
			{
				ship_left_.direction_.y_ -= 1.0f;
				up = true;
			}
			if (kb.is_down(KEYCODE_S))
			{
				ship_left_.direction_.y_ += 1.0f;
				down = true;
			}

			firetimer_ = firetimer_ - dt;
			if (kb.is_down(KEYCODE_SPACE) && firetimer_.as_seconds() < 0.0f)
			{
				firetimer_ = time(fire_rate_ms);
				vector2 pos = ship_left_.entity_.position_ + ship_left_.offset_;
				bullets_.spawn(pos, { 1.0f, 0.0f });

				space = true;
			}
			bullets_.update(dt);

			input_buffer_.push_back(input(up, down, space, dt.tick_));

			message_input inputMessage(up, down, space);
			send_input(inputMessage);

			invaders_left_.update(dt);
			ship_left_.update(dt);
			blocks_left_.update(sprite_sheet_);

			// note: remote ...
			//invaders_right_.update(dt);
			//ship_right_.update(dt);
			//blocks_right_.update(sprite_sheet_);

			// note: collision
			dynamic_array<contact> contacts;
			check_collision(bullets_, blocks_left_, contacts);
			check_collision(bullets_, blocks_right_, contacts);
			check_collision(bullets_, invaders_left_, contacts);
			check_collision(bullets_, invaders_right_, contacts);
			for (auto& c : contacts)
			{
				explosions_.spawn(c.position_);
			}
			contacts.clear();

			check_collision(bullets_, ship_left_, contacts);
			check_collision(bullets_, ship_right_, contacts);
			for (auto& c : contacts)
			{
				explosions_.spawn(c.position_);
			}
			contacts.clear();

			explosions_.update(dt);
		}

		return true;
	}

	bool space_invaders::send_input(uu::message_input& inputMessage)
	{
		uint8 array[1400] = {};
		gamma::byte_stream stream(sizeof(array), array);
		gamma::byte_stream_writer writer(stream);
		if (!inputMessage.serialize(writer))
		{
			return false;
		}
		if (!socket_.send_to(remote_, stream))
		{
			return false;
		}
		return true;
	}

	bool space_invaders::receive_input(uu::message_input& inputMessage)
	{
		uint8 array[1400] = {};
		gamma::byte_stream stream(sizeof(array), array);
		if (!socket_.recv_from(remote_, stream))
			return false;

		gamma::byte_stream_reader reader(stream);
		inputMessage.serialize(reader);
		return inputMessage.is_valid();
	}

	bool space_invaders::send_input_buffer(uu::message_input_buffer& input_buffer_message)
	{
		uint8 array[1400] = {};
		gamma::byte_stream stream(sizeof(array), array);
		gamma::byte_stream_writer writer(stream);
		if (!input_buffer_message.serialize(writer))
		{
			return false;
		}
		if (!socket_.send_to(remote_, stream))
		{
			return false;
		}
		return true;
	}

	bool space_invaders::receive_input_buffer(uu::message_input_buffer& input_buffer_message)
	{
		uint8 array[1400] = {};
		gamma::byte_stream stream(sizeof(array), array);
		if (!socket_.recv_from(remote_, stream))
			return false;

		gamma::byte_stream_reader reader(stream);
		input_buffer_message.serialize(reader);
		return true;
	}

	void space_invaders::render(render_system& rs)
	{
		rs.clear(0xff440044);
		if (state_ == GAME_STATE_INIT)
		{
			rs.draw_text(10, 10, 0xffffffff, 3, "INIT STATE");
		}
		else if (state_ == GAME_STATE_CONNECTING)
		{
			rs.draw_text(10, 10, 0xffffffff, 3, "WAITING FOR OPPONENT");
		}
		else if (state_ == GAME_STATE_PLAY)
		{
			invaders_left_.render(rs);
			invaders_right_.render(rs);
			bullets_.render(rs);
			explosions_.render(rs);
			ship_left_.render(rs);
			ship_right_.render(rs);
			blocks_left_.render(rs);
			blocks_right_.render(rs);
		}
	}
	bool space_invaders::send_connection_request()
	{
		uint8 array[1400] = {};
		gamma::byte_stream stream(sizeof(array), array);
		gamma::byte_stream_writer writer(stream);
		uu::message_connection_request message;
		if (!message.serialize(writer))
		{
			return false;
		}

		if (!socket_.send_to(remote_, stream))
		{
			return false;
		}

		return true;
	}
	bool space_invaders::send_connection_response()
	{
		uint8 array[1400] = {};
		gamma::byte_stream stream(sizeof(array), array);
		gamma::byte_stream_writer writer(stream);
		uu::message_connection_response message;
		message.random_ = 666666;
		if (!message.serialize(writer))
		{
			return false;
		}

		if (!socket_.send_to(remote_, stream))
		{
			return false;
		}

		return true;
	}

	bool space_invaders::receive_connection_request(gamma::byte_stream& stream)
	{
		gamma::byte_stream_reader reader(stream);

		uu::message_connection_request req_msg;
		if (!req_msg.serialize(reader))
		{
			return false;
		}
		if (!req_msg.is_valid())
		{
			return false;
		}
		return true;
	}

	bool space_invaders::receive_connection_response(gamma::byte_stream& stream)
	{
		gamma::byte_stream_reader reader(stream);

		uu::message_connection_response response;
		if (!response.serialize(reader))
		{
			return false;
		}
		if (response.random_ == 666666)
		{
			return true;
		}

		return false;
	}




} // !uu
