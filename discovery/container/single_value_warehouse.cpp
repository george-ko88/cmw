#include <cmw/discovery/container/single_value_warehouse.h>
#include <cmw/common/log.h>

namespace hnu {
namespace cmw {
namespace discovery{ 

//根据key往rolemap中添加元素
bool SingleValueWarehouse::Add(uint64_t key, const RolePtr& role,
                               bool ignore_if_exist) {
  WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  if (!ignore_if_exist) {
    if (roles_.find(key) != roles_.end()) {
      return false;
    }
  }
  roles_[key] = role;
  return true;
}

void SingleValueWarehouse::Clear() {
  WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  roles_.clear();
}

std::size_t SingleValueWarehouse::Size() {
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  return roles_.size();
}

void SingleValueWarehouse::Remove(uint64_t key) {
  WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  roles_.erase(key);
}

void SingleValueWarehouse::Remove(uint64_t key, const RolePtr& role) {
  WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  auto search = roles_.find(key);
  if (search == roles_.end()) {
    return;
  }
  if (!search->second->Match(role->attributes())) {
    return;
  }
  roles_.erase(search);
}

void SingleValueWarehouse::Remove(const RoleAttributes& target_attr) {
  WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  for (auto it = roles_.begin(); it != roles_.end();) {
    auto curr_role = it->second;
    if (curr_role->Match(target_attr)) {
      it = roles_.erase(it);
    } else {
      ++it;
    }
  }
}

void SingleValueWarehouse::GetAllRoles(
    std::vector<RoleAttributes>* roles_attr) {
  RETURN_IF_NULL(roles_attr);
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  for (auto& item : roles_) {
    roles_attr->emplace_back(item.second->attributes());
  }
}

void SingleValueWarehouse::GetAllRoles(std::vector<RolePtr>* roles) {
  RETURN_IF_NULL(roles);
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  for (auto& item : roles_) {
    roles->emplace_back(item.second);
  }
}


bool SingleValueWarehouse::Search(uint64_t key) {
  RolePtr role;
  return Search(key, &role);
}

bool SingleValueWarehouse::Search(uint64_t key, RolePtr* first_matched_role) {
  RETURN_VAL_IF_NULL(first_matched_role, false);
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  auto search = roles_.find(key);
  if (search == roles_.end()) {
    return false;
  }
  *first_matched_role = search->second;
  return true;
}

bool SingleValueWarehouse::Search(uint64_t key,
                                  RoleAttributes* first_matched_role_attr) {
  RETURN_VAL_IF_NULL(first_matched_role_attr, false);
  RolePtr role;
  if (!Search(key, &role)) {
    return false;
  }
  *first_matched_role_attr = role->attributes();
  return true;
}

bool SingleValueWarehouse::Search(uint64_t key,
                                  std::vector<RolePtr>* matched_roles) {
  RETURN_VAL_IF_NULL(matched_roles, false);
  RolePtr role;
  if (!Search(key, &role)) {
    return false;
  }
  matched_roles->emplace_back(role);
  return true;
}

bool SingleValueWarehouse::Search(
    uint64_t key, std::vector<RoleAttributes>* matched_roles_attr) {
  RETURN_VAL_IF_NULL(matched_roles_attr, false);
  RolePtr role;
  if (!Search(key, &role)) {
    return false;
  }
  matched_roles_attr->emplace_back(role->attributes());
  return true;
}

bool SingleValueWarehouse::Search(const RoleAttributes& target_attr) {
  RolePtr role;
  return Search(target_attr, &role);
}

bool SingleValueWarehouse::Search(const RoleAttributes& target_attr,
                                  RolePtr* first_matched_role) {
  RETURN_VAL_IF_NULL(first_matched_role, false);
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  for (auto& item : roles_) {
    if (item.second->Match(target_attr)) {
      *first_matched_role = item.second;
      return true;
    }
  }
  return false;
}

bool SingleValueWarehouse::Search(const RoleAttributes& target_attr,
                                  RoleAttributes* first_matched_role_attr) {
  RETURN_VAL_IF_NULL(first_matched_role_attr, false);
  RolePtr role;
  if (!Search(target_attr, &role)) {
    return false;
  }
  //使用默认的 = 重载，浅拷贝
  *first_matched_role_attr = role->attributes();
  return true;
}

bool SingleValueWarehouse::Search(const RoleAttributes& target_attr,
                                  std::vector<RolePtr>* matched_roles) {
  RETURN_VAL_IF_NULL(matched_roles, false);
  bool find = false;
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  for (auto& item : roles_) {
    if (item.second->Match(target_attr)) {
      matched_roles->emplace_back(item.second);
      find = true;
    }
  }
  return find;
}

bool SingleValueWarehouse::Search(
    const RoleAttributes& target_attr,
    std::vector<RoleAttributes>* matched_roles_attr) {
  RETURN_VAL_IF_NULL(matched_roles_attr, false);
  bool find = false;
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  for (auto& item : roles_) {
    if (item.second->Match(target_attr)) {
      matched_roles_attr->emplace_back(item.second->attributes());
      find = true;
    }
  }
  return find;
}


}
}
}