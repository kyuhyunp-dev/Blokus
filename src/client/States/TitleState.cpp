#include "States/TitleState.hpp"
#include "GUI/Button.hpp"
#include "GUI/TextBox.hpp"
#include "GUI/Label.hpp"
#include "Resource/ResourceHolder.hpp"
#include "ClientUtility.hpp"
#include "Network/NetworkClient.hpp"
#include "shared/Network/NetworkProtocol.hpp"

#include <spdlog/spdlog.h>


TitleState::TitleState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
	, mConnectionFuture()
	, mIsConnecting(false)
{
	const sf::Font& font = getContext().fonts->get(Fonts::Sansation);

	sf::Vector2f windowSize(context.window->getSize());
	float centerX = windowSize.x / 2.f;
	float centerY = windowSize.y / 2.f;

	// Blokem Label
	auto titleLabel = std::make_unique<GUI::Label>("BLOKEM", font, 80);
	titleLabel->setPosition(sf::Vector2f(centerX, centerY - 200.f));
	mGUIContainer.pack(std::move(titleLabel));

	// Status Label
	auto statusLabel = std::make_unique<GUI::Label>("", font, 24);
	mStatusLabel = statusLabel.get();
	mStatusLabel->setPosition(sf::Vector2f(centerX, centerY));

	// Connect to the server
	if (!context.networkClient->isConnected())
	{
		mIsConnecting = true;
		mConnectionFuture = std::async(std::launch::async, [this]()
		{
			spdlog::info("[TitleState] Attempting to connect to server");
			return getContext().networkClient->connect(IpAddress, ServerPort);
		});

		mStatusLabel->setText("Connecting...");
	}
	else
	{
		mStatusLabel->setText("");
	}

	// Create Match Button
	auto createButton = std::make_unique<GUI::Button>(
		"Create Match", font, sf::Vector2f(250.f, 60.f)
	);
	createButton->setPosition(sf::Vector2f(centerX - 150.f, centerY + 100.f));
	createButton->setCallback([this]() 
	{ 
		requestCreateMatch();	
	});
	mGUIContainer.pack(std::move(createButton));

	// Match Code Text Box
	auto codeTextBox = std::make_unique<GUI::TextBox>(
		"Enter 4 Digit Code", font, sf::Vector2f(250.f, 40.f)
	);
	GUI::TextBox* codeTextBoxPtr = codeTextBox.get();
	
	codeTextBox->setPosition(sf::Vector2f(centerX + 150.f, centerY + 50.f));
	mGUIContainer.pack(std::move(codeTextBox));

	// Join Button
	auto joinButton = std::make_unique<GUI::Button>(
		"Join Match", font, sf::Vector2f(250.f, 60.f)
	);
	joinButton->setPosition(sf::Vector2f(centerX + 150.f, centerY + 100.f));
	joinButton->setCallback([this, codeTextBoxPtr]()
	{
		std::string matchCode = codeTextBoxPtr->getText();
		if (!matchCode.empty() && matchCode.size() == CODE_SIZE)
		{
			mStatusLabel->setText("");
			requestJoinMatch(matchCode);
		}
		else if (matchCode.empty())
		{
			mStatusLabel->setText("Please enter a match code");	
		}
		else
		{
			mStatusLabel->setText("Code must be 4 digits");
		}
	});


	mGUIContainer.pack(std::move(joinButton));

	// Add status label at the end
	mGUIContainer.pack(std::move(statusLabel));
}

void TitleState::draw()
{
	getContext().window->draw(mGUIContainer);
}

bool TitleState::update(sf::Time dt)
{
	mGUIContainer.update(dt);

	if (mIsConnecting)
	{ // Still connecting to the server
		if (mConnectionFuture.valid() &&
			mConnectionFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			mIsConnecting = false;
			bool connectedSuccessfully = mConnectionFuture.get();
			
			if (!connectedSuccessfully)
			{
				mStatusLabel->setText("Could not connect to the server...");
			}
			else
			{
				mStatusLabel->setText("");
			}
		}

		return false;
	}

	// Can use network
	sf::Packet packet;
	while (getContext().networkClient->pollPacket(packet))
	{
		processPacket(packet);
	}

	return false;
}

bool TitleState::handleEvent(const sf::Event& event)
{
	auto worldMousePos = getWorldMousePosition(event);
    mGUIContainer.handleEvent(event, worldMousePos);

    if ((event.is<sf::Event::MouseMoved>() || event.is<sf::Event::MouseButtonPressed>() || event.is<sf::Event::MouseButtonReleased>()) && 
		worldMousePos)
    {
        setMouseCursor(mGUIContainer.getMouseCursorType(worldMousePos.value()).value_or(sf::Cursor::Type::Arrow));
    }

	return false;
}

void TitleState::requestCreateMatch()
{
    spdlog::info("[TitleState] Create Match Request Sent!"); 

    sf::Packet packet;
    packet << NetworkProtocol::PacketType::CreateMatch;
    
    getContext().networkClient->sendPacket(packet);
}

void TitleState::requestJoinMatch(const std::string& matchCode)
{
    spdlog::info("[TitleState] Join Match Request Sent for code: {}!", matchCode); 

    sf::Packet packet;
    NetworkProtocol::JoinMatchRequest request{ matchCode };
    packet << NetworkProtocol::PacketType::JoinMatch << request;

    getContext().networkClient->sendPacket(packet);
}

void TitleState::processPacket(sf::Packet& packet)
{
	NetworkProtocol::PacketType type;
	if (packet >> type)
	{
		switch (type)
		{
			case NetworkProtocol::PacketType::MatchJoined:
			{
				NetworkProtocol::MatchJoinedResponse response;
				if (packet >> response)
				{
					spdlog::info("[TitleState] Successfully joined match: {}", response.matchCode);
					getContext().gameSessionData->match.matchCode = response.matchCode;

					requestStackPop();
					requestStackPush(States::ID::Username);
				}
				break;
			}
			case NetworkProtocol::PacketType::MatchJoinFailed:
			{
				NetworkProtocol::MatchJoinFailedResponse response;
				if (packet >> response)
				{
					spdlog::error("[TitleState] Failed to join the match");

					switch (response.reason)
					{
						case NetworkProtocol::JoinError::AlreadyInMatch:
						{
							mStatusLabel->setText("Already in Match"); 
							break;
						}
						case NetworkProtocol::JoinError::MatchFull:
						{
							mStatusLabel->setText("Match is full");
							break;
						}
						case NetworkProtocol::JoinError::MatchNotFound:
						{
							mStatusLabel->setText("Match not found"); 
							break;
						}
						default:
						{
							mStatusLabel->setText("Unknown Error");
							break;
						}
					}
				}
				break;
			}
			default:
			{
				spdlog::warn("[TitleState] Ignored an unexpected packet type.");
				break;
			}
		}
	}
}