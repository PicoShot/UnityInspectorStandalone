#pragma once
#include "features/features.h"

enum class ScanValueType { Int, Long, Float, Double, Bool };
enum class ScanComparison { Exact, Increased, Decreased, Changed, Unchanged };

struct ScanField {
    void* fieldHandle = nullptr;
    void* classHandle = nullptr;
    void* object = nullptr;
    std::string className;
    std::string namespaze;
    std::string fieldName;
    std::string objectName;
    bool isStatic = false;
    int offset = 0;
    ScanValueType valueType = ScanValueType::Int;

    union {
        int64_t i64;
        uint64_t u64;
        float f32;
        double f64;
        bool b;
    } lastValue{};
};

class MemoryScanner final : public IFeature {
public:
    void Update(float deltaTime) override;
    void Render() override;

private:
    enum class ScanOperation { None, FirstScan, NextScan, Reset };

    ScanValueType selectedType = ScanValueType::Int;
    ScanComparison comparison = ScanComparison::Exact;
    char valueBuffer[64] = {};
    bool scanInProgress = false;
    std::string statusText;
    bool includeSystemNamespaces = false;
    bool debugLogging = false;

    std::vector<ScanField> currentResults;
    bool hasDoneFirstScan = false;

    char resultFilterBuffer[256] = {};
    int selectedResultIndex = -1;

    ScanOperation pendingOperation = ScanOperation::None;

    union {
        int64_t i64;
        uint64_t u64;
        float f32;
        double f64;
        bool b;
    } typedValue{ .i64 = 100 };

    static constexpr size_t MAX_RESULTS = 50000;
    static constexpr size_t MAX_OBJECTS_TO_SCAN = 10000;
    static constexpr int MAX_SCAN_DEPTH = 6;

    void PerformFirstScan();
    void PerformNextScan();
    void ResetScan();

    void RenderValueInput();
    void SyncValueToBuffer();

    void ScanStaticFields(std::vector<ScanField>& out);
    void ScanUnityObjectFields(std::vector<ScanField>& out);

    struct VisitedKey {
        void* address;
        void* klass;
        bool operator==(const VisitedKey& o) const noexcept {
            return address == o.address && klass == o.klass;
        }
    };
    struct VisitedKeyHash {
        size_t operator()(const VisitedKey& k) const noexcept {
            return std::hash<void*>{}(k.address) ^ (std::hash<void*>{}(k.klass) << 1);
        }
    };

    void ScanObjectInstance(void* obj, void* klass, std::vector<ScanField>& out,
        std::unordered_set<VisitedKey, VisitedKeyHash>& visited, int depth, const std::string& objName);

    bool TypeNameMatchesSearchType(const std::string& typeName) const;
    ScanValueType DetermineActualValueType(const std::string& typeName) const;

    bool ReadFieldValue(const ScanField& field, void* outValue) const;
    bool ReadStaticFieldValue(void* fieldHandle, ScanValueType type, void* outValue) const;
    bool ReadInstanceFieldValue(void* obj, int offset, ScanValueType type, void* outValue) const;

    bool GetTargetValueAsInt64(int64_t& out) const;
    bool GetTargetValueAsUInt64(uint64_t& out) const;
    bool GetTargetValueAsFloat(float& out) const;
    bool GetTargetValueAsDouble(double& out) const;
    bool GetTargetValueAsBool(bool& out) const;

    bool CompareValueWithTarget(const void* value, ScanValueType actualType) const;
    int CompareValueWithPrevious(const void* currentValue, const ScanField& field) const;

    void OpenResultInInspector(const ScanField& result);

    static const char* GetValueTypeName(ScanValueType type);
    static const char* GetComparisonName(ScanComparison comp);
    static std::string FormatFieldValue(const ScanField& field);
};
