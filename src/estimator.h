#ifndef ESTIMATOR_H_
#define ESTIMATOR_H_
#include <vector>
#include <math.h>
#include <functional>
#include "vehicle.h"
#include "snapshot.h"
#include "prediction.h"

namespace pathplanner {
  using namespace std;

  class Estimator
  {
    public:
      Estimator(bool verbose);

      virtual ~Estimator();

      double calculate_cost(double car_s, vector<snapshot> trajectory,
        map<int, vector<prediction>>predictions, CarState state);

    private:
      bool verbose = false;

      struct collision {
        bool hasCollision = false;
        int step = 1000;
      };

      struct TrajectoryData
      {
        int proposed_lane;
        int current_lane;
        double avg_speed;
        double prop_closest_approach;
        double actual_closest_approach;
        collision collides;
      };

      typedef std::function<double(const Estimator&, vector<snapshot> trajectory,
        map<int, vector<prediction>> predictions, TrajectoryData data)> DelegateType;

      vector<DelegateType> delegates = { (DelegateType)&Estimator::inefficiency_cost,
        (DelegateType)&Estimator::collision_cost,
        (DelegateType)&Estimator::buffer_cost,
        (DelegateType)&Estimator::change_lane_cost,
        (DelegateType)&Estimator::free_line_cost
      };

      // priority levels for costs
      int const COLLISION = pow(10, 6);
      int const DANGER = pow(10, 5);
      int const COMFORT = pow(10, 4);
      int const EFFICIENCY = pow(10, 3);
      double const MAX_SPEED = 49.5;
      double const DESIRED_BUFFER = 40;

      int const PLANNING_HORIZON = 2;

      double const PREDICTION_INTERVAL = 0.5;
      double const MANOEUVRE = 4;
      double const MAX_DISTANCE = 999999;

      double change_lane_cost(vector<snapshot> trajectory,
        map<int, vector<prediction>> predictions, TrajectoryData data) const;

      double inefficiency_cost(vector<snapshot> trajectory,
        map<int, vector<prediction>> predictions, TrajectoryData data) const;

      double collision_cost(vector<snapshot> trajectory,
        map<int, vector<prediction>> predictions, TrajectoryData data) const;

      double buffer_cost(vector<snapshot> trajectory,
        map<int, vector<prediction>> predictions, TrajectoryData data) const;

      double free_line_cost(vector<snapshot> trajectory,
        map<int, vector<prediction>> predictions, TrajectoryData data) const;

      TrajectoryData get_helper_data(double car_s, vector<snapshot> trajectory,
        map<int, vector<prediction>> predictions, CarState state);

      bool check_collision(double car_s, snapshot snap, prediction s_now, CarState checkstate);

      map<int, vector<prediction>> filter_predictions_by_lane(
        map<int, vector<prediction>> predictions, int lane);
  };

}
#endif //ESTIMATOR_H_