#ifndef INC_TRACK_H_
#define INC_TRACK_H_

#include "mcv_platform.h"

template <typename VAL>
struct TTrack {
private:
	struct TKeyframe {
		float time;
		VAL   value;
	};

	std::vector<TKeyframe>  keyframes;
	bool                    sorted;

public:
	TTrack() : sorted(false) { }

	void set(float t, VAL value) {
		TKeyframe k;
		k.time = t;
		k.value = value;
		keyframes.push_back(k);
		sorted = false;
	}

	VAL get(float t) const {
		/*if (!sorted)
		sort();*/

		if (keyframes.empty()) {
			return VAL();
		}
		else if (keyframes.size() == 1) {
			return keyframes[0].value;
		}
		else {
			TKeyframe curr = keyframes[0];
			for (unsigned i = 1; i < keyframes.size(); ++i) {
				const TKeyframe& next = keyframes[i];
				if (next.time > t) {
					float t_ratio = (t - curr.time) / (next.time - curr.time);
					VAL v_ratio = curr.value + (next.value - curr.value) * t_ratio;
					return v_ratio;
				}
				else {
					curr = next;
				}
			}
			return curr.value;
		}
	}

	void sort() {
		auto sorter = [](const TKeyframe& k1, const TKeyframe& k2) {
			return k1.time < k2.time;
		};
		std::sort(keyframes.begin(), keyframes.end(), sorter);
		sorted = true;
	}

	std::vector<TKeyframe>& getKeyframes() {
		return keyframes;
	}
};

#endif

