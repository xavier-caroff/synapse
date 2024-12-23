///
/// @file BaseBlock.h
///
/// Declaration of the BaseBlock class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <string>

#include "Demangle.h"
#include "IBlock.h"
#include "IManager.h"
#include "Registry.h"

namespace synapse {
namespace framework {

///
/// Base class for the blocks of a Synapse application.
///
class BaseBlock :
	public IBlock
{
	// Construction, destruction

public:

	/// Default constructor.
	///
	/// @param[in] name The name of the block.
	BaseBlock(
		const std::string& name);

	/// Destructor
	virtual ~BaseBlock();

	// Partial implementation of IBlock

public:

	/// Get the name of the block.
	///
	/// @return The name of the block.
	const std::string& name() const override final;

	/// Initialize the block before the execution.
	///
	/// @param[in] configData The configuration data of the block.
	/// @param[in] manager The manager of the block.
	void               initialize(
					  const ConfigData& configData,
					  IManager*         manager) override;

	/// Ask the block to delete itself.
	virtual void destroy() override final;

	// Accessors

public:

	/// Access to the manager.
	IManager* manager() const { return _manager; }

	// Operations

public:

	/// Read configuration data.
	///
	/// @return The configuration data of the block.
	///
	/// @throw std::runtime_error If the configuration data is invalid.
	template<class T>
	static T readConfig(
		const ConfigData& configData);

	// Private attributes

private:

	/// Name of the bloc
	std::string _name;

	/// The manager of the application.
	IManager*   _manager{ nullptr };
};

/// Create necessary declaration stuff for component registration
// clang-format off
#define DECLARE_BLOCK(className)													\
public:																				\
																					\
	static const synapse::framework::Registry::BlockDescription& description();		\
																					\
	static synapse::framework::IBlock* create(										\
		const std::string& name);
// clang-format on

/// Implement necessary declaration stuff for component registration
// clang-format off
#define IMPLEMENT_BLOCK(className)													\
																					\
const synapse::framework::Registry::BlockDescription& className::description(		\
	)																				\
{																					\
	using synapse::framework::Registry;												\
	using synapse::framework::demangle;												\
																					\
	static Registry::BlockDescription description {									\
		demangle(typeid(className).name()),											\
		className::create															\
	};																				\
																					\
	return description;																\
}																					\
																					\
synapse::framework::IBlock* className::create(										\
	const std::string& name)														\
{																					\
	return new className(name);														\
}
// clang-format on

// Read configuration data.
template<class T>
inline T BaseBlock::readConfig(
	const ConfigData& configData)
{
	T result;

	try
	{
		result = configData.get<T>();
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(std::format("invalid configuration: {}", e.what()));
	}

	return result;
}

} // namespace framework
} // namespace synapse
