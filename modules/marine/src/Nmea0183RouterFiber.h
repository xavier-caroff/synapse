///
/// @file Nmea0183RouterFiber.h
///
/// Declaration of the Nmea0183RouterFiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <synapse/framework/Fiber.h>
#include <synapse/framework/Port.h>

namespace synapse {
namespace modules {
namespace marine {

///
/// Implement a block that dispatch NMEA0183 sentences along
/// different routes given their starting pattern.
///
class Nmea0183RouterFiber :
	public synapse::framework::Fiber
{
	DECLARE_BLOCK(Nmea0183RouterFiber)

	// Définitions

public:

	/// Configuration of the block.
	struct Config
	{
		struct Route
		{
			/// Name of the output port.
			std::string              port;
			/// List of patterns to match.
			std::vector<std::string> patterns;
		};

		/// List of routes.
		std::vector<Route>         routes;

		/// Name of the fallback port.
		std::optional<std::string> fallback;
	};

	// Construction, destruction

private:

	/// Constructor.
	///
	/// @param name Name of the block.
	Nmea0183RouterFiber(
		const std::string& name);

	/// Destructor.
	virtual ~Nmea0183RouterFiber();

	// Implementation of IBlock

public:

	/// Initialize the block before the execution.
	///
	/// @param[in] configData The configuration data of the block.
	/// @param[in] manager The manager of the block.
	void initialize(
		const ConfigData&             configData,
		synapse::framework::IManager* manager) override;

	/// Ask the block to prepare to be deleted (terminate all pending operations).
	void shutdown() override final;

	// Implementation of IProducer

public:

	/// Get the list of output ports.
	///
	/// @param[in] configData The configuration data of the block.
	///
	/// @return The list of the names of the output ports.
	std::list<std::string> ports(
		const IBlock::ConfigData& configData) override final;

	// Implementation of IConsumer

public:

	/// Consume a message.
	///
	/// @param message[in] Message to be consumed.
	void consume(
		const std::shared_ptr<synapse::framework::Message>& message) override final;

	// Private definitions

private:

	friend class Nmea0183RouterFiber_node_initialize_Test;
	friend class Nmea0183RouterFiber_node_extend_Test;
	friend class Nmea0183RouterFiber_node_match_Test;

	/// One node of the finding tree.
	struct Node
	{
		/// The byte that match this node.
		uint8_t                    byte;

		/// The next node in the chain if this node matches.
		std::unique_ptr<Node>      next;

		/// The fallback node to test if the current node does not match.
		std::unique_ptr<Node>      fallback;

		/// The port to use if this node match and it is the end of the chain.
		synapse::framework::IPort* port{ nullptr };

		/// Initialize the node and create the chain.
		void                       initialize(
								  const uint8_t              bytes[],
								  size_t                     size,
								  synapse::framework::IPort* port_);

		/// Extend the chain from this node.
		void extend(
			const uint8_t              bytes[],
			size_t                     size,
			synapse::framework::IPort* port_);

		/// Check if the begining of the provided data matches one of the patterns of this node.
		synapse::framework::IPort* match(
			const uint8_t bytes[],
			size_t        size) const;
	};

	// Private attributes

private:

	/// The configuration data.
	Config                     _config;

	/// The route node of the finding tree.
	std::unique_ptr<Node>      _root;

	/// The fallback port (may be nullptr).
	synapse::framework::IPort* _fallback{ nullptr };
};

} // namespace marine
} // namespace modules
} // namespace synapse
