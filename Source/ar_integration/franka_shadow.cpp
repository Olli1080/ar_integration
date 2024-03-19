#include "franka_shadow.h"

void U_franka_shadow_controller::Tick(float DeltaSeconds)
{
	last_update_ = FDateTime::UtcNow() + look_ahead;
	advance_to_present();

	if (!IsValid(franka_))
		return;

	const auto& current_joints_opt = current_joints_interp();
	if (!current_joints_opt.IsSet())
		return;

	franka_->SetJoints(current_joints_opt.GetValue());
}

void U_franka_shadow_controller::set_visual_plan(const TArray<F_joints_synced>& plan)
{
	const auto current_tp = FDateTime::UtcNow();

	if (plan.IsEmpty() || plan.Last().time_stamp < current_tp)
		return;

	plan_ = plan;
	idx = 0;

	advance_to_present();
}

TOptional<F_joints_synced> U_franka_shadow_controller::current_sync_joints() const
{
	if (done())
		return {};

	return plan_[idx];
}

TOptional<FFrankaJoints> U_franka_shadow_controller::current_joints_interp() const
{
	const auto sync_joints_opt = current_sync_joints();
	if (!sync_joints_opt.IsSet())
		return {};

	const auto& [joints, tp] = sync_joints_opt.GetValue();
	
	float progress = FTimespan::Ratio(last_update_ - tp, plan_[idx + 1].time_stamp - tp);
	//ensure division results in sound values
	progress = std::max(std::min(progress, 1.f), 0.f);

	return FMath::Lerp(joints, plan_[idx + 1].joints, progress);
}

void U_franka_shadow_controller::set_robot(AFranka* franka)
{
	franka_ = franka;
}

bool U_franka_shadow_controller::done() const
{
	return idx == plan_.Num();
}

void U_franka_shadow_controller::clear_Implementation()
{
	plan_.Empty();
}

void U_franka_shadow_controller::set_visibility_Implementation(Visual_Change vis_change)
{
	switch (vis_change)
	{
	case ENABLED:
		franka_->SetHidden(false);
		break;
	case DISABLED:
		franka_->SetHidden(true);
		break;
	case REVOKED:
		clear_Implementation();
		break;
	}
}

void U_franka_shadow_controller::advance_to_present()
{
	for (; idx < plan_.Num() && plan_[idx].time_stamp < last_update_; ++idx);
}
