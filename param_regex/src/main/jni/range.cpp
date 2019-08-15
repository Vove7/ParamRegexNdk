
class Range {
private:
    int begin;
    int end;

public:
    Range(int b, int e) {
        begin = b;
        end = e;
    }

    bool contains(int value) {
        return begin <= value && value <= end;
    }
};