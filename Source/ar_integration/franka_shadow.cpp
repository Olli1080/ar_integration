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
	last_update_ = FDateTime::UtcNow() + look_ahead;

	if (plan.IsEmpty() || plan.Last().time_stamp < last_update_)
		return;

	plan_ = plan;
	idx = 0;

	advance_to_present();
}

TOptional<F_joints_synced> U_franka_shadow_controller::current_sync_joints() const
{
	if (done())
		return {};
	if (plan_[0].time_stamp > last_update_)
		return {};

	return plan_[idx - 1];
}

TOptional<FFrankaJoints> U_franka_shadow_controller::current_joints_interp() const
{
	const auto sync_joints_opt = current_sync_joints();
	if (!sync_joints_opt.IsSet())
		return {};

	const auto& [joints, tp] = sync_joints_opt.GetValue();
	
	float progress = FTimespan::Ratio(last_update_ - tp, plan_[idx].time_stamp - tp);
	//ensure division results in sound values
	progress = std::max(std::min(progress, 1.f), 0.f);

	return FMath::Lerp(joints, plan_[idx].joints, progress);
}

void U_franka_shadow_controller::set_robot(AFranka* franka)
{
	franka_ = franka;
}

bool U_franka_shadow_controller::done() const
{
	return plan_.IsEmpty() || plan_.Last().time_stamp < last_update_;
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
		franka_->SetActorHiddenInGame(false);
		break;
	case DISABLED:
		franka_->SetActorHiddenInGame(true);
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
