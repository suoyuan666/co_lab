#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using reslist = std::vector<int>;

constexpr int RES_SIZE = 3;
constexpr int P_SZIE = 5;

class Process {
 private:
  reslist max_;
  reslist need_;
  reslist allocation_;
  reslist request_;
  bool finish_{false};

 public:
  static std::unique_ptr<const reslist> total;
  static std::shared_ptr<reslist> available;

  Process()
      : allocation_(reslist(RES_SIZE)),
        max_(reslist(RES_SIZE)),
        request_(reslist(RES_SIZE)),
        need_(reslist(RES_SIZE)) {}

  Process(reslist &&max, reslist &&allocation)
      : allocation_(std::move(allocation)),
        max_(std::move(max)),
        request_(reslist(RES_SIZE)) {
    reslist tmp = reslist(RES_SIZE);
    std::transform(max_.begin(), max_.end(), allocation_.begin(), tmp.begin(),
                   [](int arg1, int arg2) { return arg1 - arg2; });
    need_ = std::move(tmp);
  }

  static auto reslist2str(const reslist *val) -> std::string {
    std::string str;
#pragma unroll RES_SIZE
    for (auto val : *val) {
      str += std::to_string(val);
      str += ' ';
    }
    return str;
  }

  auto set_request(int value, int index) -> void { request_[index] = value; }
  auto set_allocation(int value, int index) -> void {
    allocation_[index] = value;
  }

  auto add_need(int value, int index) -> void {
    need_[index] = need_[index] + value;
  }

  [[nodiscard]] auto get_need() const -> reslist { return this->need_; }
  [[nodiscard]] auto get_max() const -> reslist { return this->max_; }
  [[nodiscard]] auto get_allocation() const -> reslist {
    return this->allocation_;
  }
  [[nodiscard]] auto get_request() const -> reslist { return this->request_; }

  [[nodiscard]] auto get_need2str() const -> std::string {
    return reslist2str(&need_);
  }
  [[nodiscard]] auto get_max2str() const -> std::string {
    return reslist2str(&max_);
  }
  [[nodiscard]] auto get_allocation2str() const -> std::string {
    return reslist2str(&allocation_);
  }
  [[nodiscard]] auto get_request2str() const -> std::string {
    return reslist2str(&request_);
  }
};

reslist tmp{10, 5, 7};
std::unique_ptr<const reslist> Process::total =
    std::make_unique<reslist>(std::move(tmp));
std::shared_ptr<reslist> Process::available =
    std::make_shared<reslist>(RES_SIZE);

auto ck4sec(std::array<Process, P_SZIE> pslist, int index) -> bool;
void ck4sec_core(std::array<Process, P_SZIE> pslist,
                 const std::shared_ptr<std::vector<int>> &work,
                 std::vector<bool> &finish, std::vector<int> &safeSequence);

auto main() -> int {
  std::array<Process, P_SZIE> pslist{{{{7, 5, 3}, {0, 1, 0}},
                                      {{3, 2, 2}, {2, 0, 0}},
                                      {{9, 0, 2}, {3, 0, 2}},
                                      {{2, 2, 2}, {2, 1, 1}},
                                      {{4, 3, 3}, {0, 0, 2}}}};

  std::cout << "当前各个进程的需求和占有情况:\n";
  std::cout << "    work    need    alloc" << std::endl;
#pragma unroll P_SZIE
  for (int i = 0; i < P_SZIE; ++i) {
    std::cout << "P" << i << "  " << pslist.at(i).get_max2str() << "  "
              << pslist.at(i).get_need2str() << "  "
              << pslist.at(i).get_allocation2str() << '\n';
  }

  {
    reslist tmp(RES_SIZE);
#pragma unroll RES_SIZE
    for (int i = 0; i < RES_SIZE; ++i) {
#pragma unroll RES_SIZE
      for (auto &val : pslist) {
        tmp.at(i) += val.get_allocation().at(i);
      }
    }
    std::transform(Process::total->begin(), Process::total->end(), tmp.begin(),
                   Process::available->begin(),
                   [](int arg1, int arg2) { return arg1 - arg2; });
  }

  std::cout << "请输入当前要请求资源的进程编号:" << std::endl;
  int pid{0};
  {
    std::string tmp;
    std::getline(std::cin, tmp);
    std::istringstream iss{tmp};

    if (!(iss >> pid)) {
      throw std::invalid_argument("错误的输入");
    }
  }

  std::cout << "现在输入该进程的请求序列(例: 1 2 3)" << std::endl;

  {
    int req = 0;
    std::string tmp;
    std::getline(std::cin, tmp);
    std::istringstream iss{tmp};

#pragma unroll RES_SIZE
    for (int i = 0; i < RES_SIZE; ++i) {
      if (iss >> req) {
        pslist.at(pid).set_request(req, i);
      } else {
        throw std::invalid_argument("错误的输入");
      }
    }
  }

  std::cout << "可能的安全序列情况:" << std::endl;

  if (!ck4sec(pslist, pid)) {
    std::cerr << "无安全序列" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

auto ck4sec(std::array<Process, P_SZIE> pslist, int index) -> bool {
#pragma unroll RES_SIZE
  for (int i = 0; i < RES_SIZE; ++i) {
    if (pslist.at(index).get_request().at(i) >
        pslist.at(index).get_need().at(i)) {
      std::cerr << "请求的" << i << "号资源数量超过当前进程总共所需要的资源\n";
      return false;
    }
    if (pslist.at(index).get_request().at(i) > Process::available->at(i)) {
      std::cerr << "请求的" << i << "号资源数量超过当前系统空闲的资源\n";
      return false;
    }
  }

  std::vector<int> safe_sequence;
  std::vector<bool> finish(P_SZIE, false);

#pragma unroll RES_SIZE
  for (int i = 0; i < RES_SIZE; i++) {
    auto val = pslist.at(index).get_request().at(i);
    Process::available->at(i) -= val;
    pslist.at(index).set_allocation(val, i);
    pslist.at(index).add_need(-val, i);
  }

  ck4sec_core(pslist, Process::available, finish, safe_sequence);
#pragma unroll RES_SIZE
  for (int i = 0; i < RES_SIZE; i++) {
    auto val = pslist.at(index).get_request().at(i);
    Process::available->at(i) += val;
    pslist.at(index).set_allocation(-val, i);
    pslist.at(index).add_need(val, i);
  }

  return true;
}

void ck4sec_core(std::array<Process, P_SZIE> pslist,
                 const std::shared_ptr<std::vector<int>> &work,
                 std::vector<bool> &finish, std::vector<int> &safeSequence) {
  bool all_finished = true;
  for (int i = 0; i < P_SZIE; i++) {
    if (!finish[i]) {
      bool can_allocate = true;
#pragma unroll RES_SIZE
      for (int j = 0; j < RES_SIZE; j++) {
        if (pslist.at(i).get_need().at(j) > work->at(j)) {
          can_allocate = false;
          break;
        }
      }
      if (can_allocate) {
        all_finished = false;
        finish[i] = true;
        safeSequence.push_back(i);
#pragma unroll RES_SIZE
        for (int j = 0; j < RES_SIZE; j++) {
          work->at(j) += pslist.at(i).get_allocation().at(j);
        }
        ck4sec_core(pslist, work, finish, safeSequence);
        finish[i] = false;
        safeSequence.pop_back();
#pragma unroll RES_SIZE
        for (int j = 0; j < RES_SIZE; j++) {
          work->at(j) -= pslist.at(i).get_allocation().at(j);
        }
      }
    }
  }
  if (all_finished) {
#pragma unroll 3
    for (auto val : safeSequence) {
      if (val == *(safeSequence.end() - 1)) {
        std::cout << "P" << val << "\n";
        break;
      }
      std::cout << "P" << val << " -> ";
    }
  }
}