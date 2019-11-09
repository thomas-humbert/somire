#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "value.hpp"
#include "gc.hpp"

constexpr std::array<uint8_t, 8> magicBytes = { 'S','o','m','i','r','&', 0, 1 };

enum class Opcode : uint8_t {
	NO_OP,
	CONSTANT,
	UNI_MINUS,
	BIN_PLUS
};

std::string opcodeDesc(Opcode opcode);

std::array<uint8_t, 4> serializeUInt(uint32_t x);
uint32_t parseUInt(std::array<uint8_t, 4> b);

class Chunk {
public:
	GC::GCVector<Value>* constants;
	std::vector<uint8_t> bytecode;
	
	Chunk();
	~Chunk();
	
	template <typename O>
	void writeToFile(O& output);
	
	template <typename I>
	static std::unique_ptr<Chunk> loadFromFile(I& input);
	
private:
	template <typename O>
	void writeConstantToFile(O& output, Value& val);
	
	template <typename I>
	void loadConstantFromFile(I& input);
};

#include "chunk.tpp"
