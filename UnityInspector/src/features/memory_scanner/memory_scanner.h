#pragma once
#include "features/features.h"
#include "helper/helper.h"

enum class ScanValueType
{
	Int,
	Long,
	Float,
	Double,
	Bool
};

enum class ScanComparison
{
	Exact,
	Increased,
	Decreased,
	Changed,
	Unchanged
};

enum class ActualFieldType
{
	Byte,
	SByte,
	Short,
	UShort,
	Int,
	UInt,
	Long,
	ULong,
	Float,
	Double,
	Bool
};

struct ScanField
{
	void* fieldHandle = nullptr;
	void* classHandle = nullptr;
	void* object = nullptr;
	void* owner = nullptr;
	std::string className;
	std::string namespaze;
	std::string fieldName;
	std::string objectName;
	bool isStatic = false;
	int offset = 0;
	ScanValueType valueType = ScanValueType::Int;
	ActualFieldType actualType = ActualFieldType::Int;

	union ValUnion
	{
		int64_t i64;
		uint64_t u64;
		float f32;
		double f64;
		bool b;
	};

	ValUnion lastValue{};
	ValUnion prevValue{};
	bool hasPrevValue = false;
};

class MemoryScanner final : public IFeature
{
public:
	void Update(float deltaTime) override;
	void Render() override;

private:
	enum class ScanOperation
	{
		None,
		FirstScan,
		NextScan,
		Reset
	};

	ScanValueType selectedType = ScanValueType::Int;
	ScanComparison comparison = ScanComparison::Exact;
	char m_valueBuffer[64] = {};
	bool scanInProgress = false;
	std::string statusText;
	bool includeSystemNamespaces = false;
	bool debugLogging = false;

	std::vector<ScanField> currentResults;
	bool hasDoneFirstScan = false;

	char resultFilterBuffer[256] = {};
	int selectedResultIndex = -1;

	ScanOperation pendingOperation = ScanOperation::None;

	union
	{
		int64_t i64;
		uint64_t u64;
		float f32;
		double f64;
		bool b;
	} typedValue{.i64 = 100};

	static constexpr size_t MAX_RESULTS = 5000000;
	static constexpr size_t MAX_OBJECTS_TO_SCAN = 100000;
	static constexpr int MAX_SCAN_DEPTH = 20;

	enum class ScanPhase
	{
		Idle,
		Objects,
		Next
	};

	void StartFirstScan();
	bool StepObjectScan(std::chrono::steady_clock::time_point deadline);
	bool StepNextScan(std::chrono::steady_clock::time_point deadline);
	void FinishScan();
	void ResetScan();

	void RenderValueInput();
	void SyncValueToBuffer();

	void ScanStaticFields(std::vector<ScanField>& out) const;

	struct VisitedKey
	{
		void* address;
		void* klass;

		bool operator==(const VisitedKey& o) const noexcept { return address == o.address && klass == o.klass; }
	};

	struct VisitedKeyHash
	{
		size_t operator()(const VisitedKey& k) const noexcept
		{
			return std::hash<void*>{}(k.address) ^ (std::hash<void*>{}(k.klass) << 1);
		}
	};

	void ScanObjectInstance(void* obj, void* klass, std::vector<ScanField>& out,
	                        std::unordered_set<VisitedKey, VisitedKeyHash>& visited, int depth,
	                        const std::string& objName, void* owner);

	bool TypeNameMatchesSearchType(const std::string& typeName) const;
	ActualFieldType DetermineActualFieldType(const std::string& typeName) const;

	bool ReadFieldValue(const ScanField& field, void* outValue) const;
	bool ReadStaticFieldValue(void* fieldHandle, void* outValue) const;
	bool ReadInstanceFieldValue(void* obj, int offset, ActualFieldType type, void* outValue) const;
	bool WriteFieldValue(const ScanField& field, const void* valueBuffer) const;

	bool GetTargetValueAsInt64(int64_t& out) const;
	bool GetTargetValueAsUInt64(uint64_t& out) const;
	bool GetTargetValueAsFloat(float& out) const;
	bool GetTargetValueAsDouble(double& out) const;
	bool GetTargetValueAsBool(bool& out) const;

	bool CompareValueWithTarget(const void* value, ActualFieldType actualType) const;
	int CompareValueWithPrevious(const void* currentValue, const ScanField& field) const;

	void OpenResultInInspector(const ScanField& result) const;
	std::vector<void*> GatherUnityObjects() const;

	static const char* GetValueTypeName(ScanValueType type);
	static const char* GetActualFieldTypeName(ActualFieldType type);
	static const char* GetComparisonName(ScanComparison comp);
	static std::string FormatFieldValue(const ScanField& field);
	static std::string FormatValue(const ScanField::ValUnion& val, ActualFieldType type);

	std::vector<void*> objectsGathered;
	std::vector<Helper::GcPin> gatheredPins;
	std::unordered_map<void*, Helper::GcPin> resultPins;

	ScanField::ValUnion editValue{.i64 = 0};
	struct PendingWrite
	{
		ScanField field;
		ScanField::ValUnion value;
	};
	std::optional<PendingWrite> pendingWrite;

	ScanPhase scanPhase = ScanPhase::Idle;
	size_t scanCursor = 0;
	std::vector<ScanField> scanBuffer;
	std::unordered_set<VisitedKey, VisitedKeyHash> scanVisited;
	bool stopRequested = false;
};
